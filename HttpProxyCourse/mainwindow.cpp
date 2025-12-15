#include "mainwindow.h"
#include <QApplication>

// Константа для имени файла курса
static const QString COURSE_DATA_FILE = "course.dat";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_adminWidget(nullptr)
    , m_studentProfileWidget(nullptr)
{
    setWindowTitle("HTTP Proxy Training System");
    resize(900, 650);

    setupMenu();

    m_stackedWidget = new QStackedWidget(this);

    m_loginWidget = new LoginWidget(this);
    m_topicWidget = new TopicSelectionWidget(this);
    m_topicViewWidget = new TopicViewWidget(this);
    m_testWidget = new TestWidget(this);

    m_stackedWidget->addWidget(m_loginWidget);
    m_stackedWidget->addWidget(m_topicWidget);
    m_stackedWidget->addWidget(m_topicViewWidget);
    m_stackedWidget->addWidget(m_testWidget);

    setCentralWidget(m_stackedWidget);

    // Новые сигналы для аутентификации
    connect(m_loginWidget, &LoginWidget::userAuthenticated,
            this, &MainWindow::handleUserAuthenticated);
    
    // Сигналы для обратной совместимости
    connect(m_loginWidget, &LoginWidget::startStudentSession,
            this, &MainWindow::handleStudentStart);
    connect(m_loginWidget, &LoginWidget::adminLoginAttempt,
            this, &MainWindow::handleAdminLogin);

    connect(m_topicWidget, &TopicSelectionWidget::logoutRequested,
            this, &MainWindow::handleLogout);
    connect(m_topicWidget, &TopicSelectionWidget::topicSelected,
            this, &MainWindow::onTopicSelected);
    connect(m_topicWidget, &TopicSelectionWidget::profileRequested,
            this, &MainWindow::onShowProfileRequested);

    connect(m_topicViewWidget, &TopicViewWidget::backRequested,
            this, [this](){ m_stackedWidget->setCurrentWidget(m_topicWidget); });
    connect(m_topicViewWidget, &TopicViewWidget::startTestRequested,
            this, &MainWindow::onStartTestRequested);

    connect(m_testWidget, &TestWidget::answerSubmitted,
            this, &MainWindow::onAnswerSubmitted);
    connect(m_testWidget, &TestWidget::testFinished,
            this, &MainWindow::onTestFinished);

    loadCourseData();
}

MainWindow::~MainWindow() {
    // Деструктор явно определен для корректной очистки ресурсов
    // Qt автоматически удаляет дочерние виджеты
}

void MainWindow::setupMenu() {
    QMenuBar* bar = menuBar();
    QMenu* helpMenu = bar->addMenu("Справка");
    QAction* aboutAction = helpMenu->addAction("О программе");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::showAboutDialog() {
    QString aboutText =
        "Курсовая работа: Обучающая система HTTP Proxy.\n"
        "Выполнил: Несов Кирилл Алексеевич, студент группы АБс-424, АВТФ, НГТУ\n"
        "Год: 2025\n\n"
        "Программа предназначена для изучения и тестирования знаний по теме прокси-серверов.";

    QMessageBox::about(this, "О программе", aboutText);
}

void MainWindow::loadCourseData() {
    try {
        m_sessionManager.loadCourse(COURSE_DATA_FILE);
    } catch (const std::exception& e) {
        qWarning() << "Could not load course data:" << e.what();
        QMessageBox::warning(this, "Ошибка загрузки", 
                           QString("Не удалось загрузить данные курса: %1").arg(e.what()));
    } catch (...) {
        qWarning() << "Unknown error occurred while loading course data";
        QMessageBox::critical(this, "Критическая ошибка", 
                            "Произошла неизвестная ошибка при загрузке курса");
    }
}

void MainWindow::handleUserAuthenticated(const User& user) {
    if (!m_sessionManager.isCourseLoaded()) {
        QMessageBox::critical(this, "Ошибка", "Курс не загружен. Обратитесь к администратору.");
        return;
    }
    
    // Установка пользователя в сессии
    m_sessionManager.setCurrentUser(user);
    
    if (user.isAdmin()) {
        // Администратор - переход к админ-панели
        if (!m_adminWidget) {
            m_adminWidget = new AdminWidget(&m_sessionManager.getMutableCourse(), this);
            connect(m_adminWidget, &AdminWidget::backRequested,
                    this, &MainWindow::onAdminBackRequested);
            m_stackedWidget->addWidget(m_adminWidget);
        }
        m_adminWidget->setCurrentUser(user); // Установка пользователя для проверки прав
        m_stackedWidget->setCurrentWidget(m_adminWidget);
    } else {
        // Студент - переход к выбору тем
        m_topicWidget->setTopics(m_sessionManager.getCourse().topics);
        m_stackedWidget->setCurrentWidget(m_topicWidget);
    }
}

void MainWindow::handleStudentStart() {
    if (!m_sessionManager.isCourseLoaded()) {
        QMessageBox::critical(this, "Ошибка", "Курс не загружен. Обратитесь к администратору.");
        return;
    }
    
    // Создание гостевого пользователя
    User guestUser(-1, "guest", "Гость", "student");
    m_sessionManager.setCurrentUser(guestUser);
    
    m_topicWidget->setTopics(m_sessionManager.getCourse().topics);
    m_stackedWidget->setCurrentWidget(m_topicWidget);
}

void MainWindow::handleAdminLogin(const QString& password) {
    if (AuthService::checkAdminPassword(password)) {
        if (!m_adminWidget) {
            // Передаем изменяемую ссылку на курс через SessionManager
            m_adminWidget = new AdminWidget(&m_sessionManager.getMutableCourse(), this);
            connect(m_adminWidget, &AdminWidget::backRequested,
                    this, &MainWindow::onAdminBackRequested);
            m_stackedWidget->addWidget(m_adminWidget);
        }
        m_stackedWidget->setCurrentWidget(m_adminWidget);
    } else {
        QMessageBox::warning(this, "Ошибка доступа", "Неверный пароль администратора.");
    }
}

void MainWindow::handleLogout() {
    m_stackedWidget->setCurrentWidget(m_loginWidget);
}

void MainWindow::onAdminBackRequested() {
    loadCourseData();
    m_stackedWidget->setCurrentWidget(m_loginWidget);
}

void MainWindow::onTopicSelected(int index) {
    try {
        m_sessionManager.startTopic(index);

        Topic* topic = m_sessionManager.getCurrentTopic();
        if (topic) {
            m_topicViewWidget->showTopic(*topic, index);
            m_stackedWidget->setCurrentWidget(m_topicViewWidget);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть тему: %1").arg(e.what()));
    }
}

void MainWindow::onStartTestRequested() {
    try {
        Topic* currentTopic = m_sessionManager.getCurrentTopic();
        if (!currentTopic || currentTopic->questions.isEmpty()) {
            QMessageBox::warning(this, "Внимание", "В данной теме отсутствуют вопросы для тестирования.");
            return;
        }
        
        User currentUser = m_sessionManager.getCurrentUser();
        if (!currentUser.isValid()) {
            QMessageBox::critical(this, "Ошибка", "Пользователь не авторизован.");
            return;
        }
        
        // Запуск теста с вопросами текущей темы
        m_testWidget->startTest(currentTopic->questions, currentUser);
        m_stackedWidget->setCurrentWidget(m_testWidget);
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка теста", e.what());
    }
}

void MainWindow::onAnswerSubmitted(int answerIndex) {
    try {
        auto result = m_sessionManager.submitAnswer(answerIndex);

        switch (result) {
        case SessionManager::SubmitResult::Correct:
            QMessageBox::information(this, "Верно", "Правильный ответ!");
            {
                Question* nextQ = m_sessionManager.getCurrentQuestion();
                if (nextQ) {
                    m_testWidget->showQuestion(*nextQ);
                }
            }
            break;

        case SessionManager::SubmitResult::Wrong:
            QMessageBox::warning(this, "Ошибка", "Неверный ответ. Попробуйте снова.");
            break;

        case SessionManager::SubmitResult::FailRelearn:
            QMessageBox::critical(this, "Тест провален",
                                  "Допущено критическое количество ошибок (3).\n"
                                  "Вам необходимо повторить теоретический материал.");
            m_stackedWidget->setCurrentWidget(m_topicViewWidget);
            break;

        case SessionManager::SubmitResult::TopicFinished:
            QMessageBox::information(this, "Успех", "Тема успешно пройдена!");
            m_sessionManager.saveProgress(); // Сохранение прогресса
            m_topicWidget->setTopics(m_sessionManager.getCourse().topics);
            m_stackedWidget->setCurrentWidget(m_topicWidget);
            break;

        case SessionManager::SubmitResult::CourseFinished:
            QMessageBox::information(this, "Поздравляем", "Вы успешно завершили весь курс!");
            m_sessionManager.saveProgress(); // Сохранение прогресса
            m_stackedWidget->setCurrentWidget(m_topicWidget);
            break;
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Системная ошибка", e.what());
        m_stackedWidget->setCurrentWidget(m_topicWidget);
    }
}

void MainWindow::onTestFinished(int score, int maxScore, bool timeExpired) {
    QString message;
    double percentage = maxScore > 0 ? (static_cast<double>(score) / maxScore) * 100.0 : 0.0;
    
    if (timeExpired) {
        message = QString("Время тестирования истекло!\n\n"
                         "Результат: %1 из %2 (%3%)")
                         .arg(score).arg(maxScore).arg(percentage, 0, 'f', 1);
    } else {
        message = QString("Тест завершен!\n\n"
                         "Результат: %1 из %2 (%3%)")
                         .arg(score).arg(maxScore).arg(percentage, 0, 'f', 1);
    }
    
    // Определение оценки
    QString grade;
    if (percentage >= 90) {
        grade = "Отлично";
    } else if (percentage >= 75) {
        grade = "Хорошо";
    } else if (percentage >= 60) {
        grade = "Удовлетворительно";
    } else {
        grade = "Неудовлетворительно";
    }
    
    message += QString("\nОценка: %1").arg(grade);
    
    QMessageBox::information(this, "Результат тестирования", message);
    
    // Возврат к выбору тем
    m_stackedWidget->setCurrentWidget(m_topicWidget);
}

void MainWindow::onShowProfileRequested() {
    // Проверяем, что пользователь аутентифицирован
    if (!m_sessionManager.hasUser()) {
        QMessageBox::warning(this, "Доступ запрещен", 
                           "Профиль доступен только для зарегистрированных пользователей.");
        return;
    }

    User currentUser = m_sessionManager.getCurrentUser();
    
    // Создаем виджет профиля, если он еще не создан
    if (!m_studentProfileWidget) {
        m_studentProfileWidget = new StudentProfileWidget(this);
        connect(m_studentProfileWidget, &StudentProfileWidget::backRequested,
                this, [this]() { 
                    m_stackedWidget->setCurrentWidget(m_topicWidget); 
                });
        m_stackedWidget->addWidget(m_studentProfileWidget);
    }
    
    // Устанавливаем текущего пользователя и показываем профиль
    m_studentProfileWidget->setCurrentUser(currentUser);
    m_stackedWidget->setCurrentWidget(m_studentProfileWidget);
}
