#include "AppController.h"
#include "LoginWidget.h"
#include "TopicSelectionWidget.h"
#include "TopicViewWidget.h"
#include "TestWidget.h"
#include "AdminWidget.h"
#include "StudentProfileWidget.h"
#include "DatabaseManager.h"
#include "ProgressDao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

const int AppController::TEST_TIME_LIMIT_MS;

AppController::AppController(QStackedWidget* stackedWidget, QObject* parent)
    : QObject(parent)
    , m_courseModel(nullptr)
    , m_testResultsModel(nullptr)
    , m_stackedWidget(stackedWidget)
    , m_loginWidget(nullptr)
    , m_topicWidget(nullptr)
    , m_topicViewWidget(nullptr)
    , m_testWidget(nullptr)
    , m_adminWidget(nullptr)
    , m_profileWidget(nullptr)
    , m_testTimer(nullptr)
    , m_currentTopicIndex(-1)
    , m_currentQuestionIndex(-1)
    , m_correctAnswers(0)
{
    // Инициализация моделей данных
    m_courseModel = new CourseModel(this);
    m_testResultsModel = new TestResultsModel(this);
    
    // Инициализация таймера тестирования
    m_testTimer = new QTimer(this);
    m_testTimer->setSingleShot(true);
    connect(m_testTimer, &QTimer::timeout, this, &AppController::handleTestTimeout);
}

bool AppController::initialize() {
    // Загрузка данных курса
    const QString courseDataFile = "course.dat";
    if (!m_courseModel->loadCourse(courseDataFile)) {
        qCritical() << "Failed to load course data";
        return false;
    }
    
    // Создание и настройка виджетов
    setupViews();
    
    // Подключение сигналов и слотов
    connectSignals();
    
    // Показываем экран авторизации
    switchToView(m_loginWidget);
    
    return true;
}

void AppController::setupViews() {
    // Создание виджетов с правильной системой родителей
    m_loginWidget = new LoginWidget(m_stackedWidget);
    m_topicWidget = new TopicSelectionWidget(m_stackedWidget);
    m_topicViewWidget = new TopicViewWidget(m_stackedWidget);
    m_testWidget = new TestWidget(m_stackedWidget);
    m_profileWidget = new StudentProfileWidget(m_stackedWidget);
    
    // Добавление виджетов в стек
    m_stackedWidget->addWidget(m_loginWidget);
    m_stackedWidget->addWidget(m_topicWidget);
    m_stackedWidget->addWidget(m_topicViewWidget);
    m_stackedWidget->addWidget(m_testWidget);
    m_stackedWidget->addWidget(m_profileWidget);
    
    // Настройка виджета выбора тем
    m_topicWidget->setTopics(m_courseModel->getTopics());
}

void AppController::connectSignals() {
    // Подключение сигналов модели курса
    connect(m_courseModel, &CourseModel::errorOccurred,
            this, &AppController::handleCourseModelError);
    
    // Подключение сигналов модели результатов
    connect(m_testResultsModel, &TestResultsModel::databaseError,
            this, &AppController::handleTestResultsError);
    
    // Подключение сигналов виджета авторизации
    connect(m_loginWidget, &LoginWidget::userAuthenticated,
            this, &AppController::handleUserAuthenticated);
    
    // Подключение сигналов виджета выбора тем
    connect(m_topicWidget, &TopicSelectionWidget::topicSelected,
            this, &AppController::handleTopicSelected);
    connect(m_topicWidget, &TopicSelectionWidget::logoutRequested,
            this, &AppController::handleLogout);
    connect(m_topicWidget, &TopicSelectionWidget::profileRequested,
            this, &AppController::handleShowProfile);
    
    // Подключение сигналов виджета просмотра темы
    connect(m_topicViewWidget, &TopicViewWidget::startTestRequested,
            this, &AppController::handleStartTest);
    connect(m_topicViewWidget, &TopicViewWidget::backRequested,
            this, [this]() { switchToView(m_topicWidget); });
    connect(m_topicViewWidget, &TopicViewWidget::progressUpdateRequested,
            this, &AppController::handleProgressUpdate);
    
    // Подключение сигналов виджета тестирования
    connect(m_testWidget, &TestWidget::answerSubmitted,
            this, &AppController::handleAnswerSubmitted);
    connect(m_testWidget, &TestWidget::testFinished,
            this, [this](int score, int maxScore, bool timeExpired) {
                QString message = QString("Тест завершен!\nПравильных ответов: %1 из %2\nПроцент: %3%")
                    .arg(score).arg(maxScore).arg(qRound((double)score / maxScore * 100));
                
                if (timeExpired) {
                    message.prepend("Время истекло!\n");
                }
                
                emit showMessage("Результат теста", message);
                switchToView(m_topicWidget);
            });
    
    // Подключение сигналов профиля студента
    connect(m_profileWidget, &StudentProfileWidget::backRequested,
            this, [this]() { switchToView(m_topicWidget); });
}

void AppController::handleUserAuthenticated(const User& user) {
    m_sessionManager.setCurrentUser(user);
    
    if (user.isAdmin()) {
        // Создаем виджет администратора при необходимости
        if (!m_adminWidget) {
            // Для администратора создаем виджет без прямого доступа к данным курса
            m_adminWidget = new AdminWidget(nullptr, m_stackedWidget);
            m_stackedWidget->addWidget(m_adminWidget);
            
            connect(m_adminWidget, &AdminWidget::backRequested,
                    this, &AppController::handleAdminBack);
        }
        
        m_adminWidget->setCurrentUser(user);
        switchToView(m_adminWidget);
    } else {
        // Загружаем прогресс студента
        int lastTopicId = loadStudentProgress(user.id);
        if (lastTopicId >= 0) {
            m_topicWidget->setLastStudiedTopic(lastTopicId);
        }
        
        switchToView(m_topicWidget);
    }
}

void AppController::handleLogout() {
    m_sessionManager.clearSession();
    switchToView(m_loginWidget);
}

void AppController::handleTopicSelected(int topicIndex) {
    const Topic* topic = m_courseModel->getTopic(topicIndex);
    if (!topic) {
        emit errorOccurred("Тема не найдена");
        return;
    }
    
    m_currentTopicIndex = topicIndex;
    m_topicViewWidget->showTopic(*topic, topicIndex);
    switchToView(m_topicViewWidget);
    
    // Сохраняем прогресс студента
    User currentUser = m_sessionManager.getCurrentUser();
    if (currentUser.isValid() && !currentUser.isAdmin()) {
        saveStudentProgress(currentUser.id, topicIndex);
    }
}

void AppController::handleStartTest() {
    const Topic* topic = m_courseModel->getTopic(m_currentTopicIndex);
    if (!topic || topic->questions.isEmpty()) {
        emit errorOccurred("Нет вопросов для тестирования по данной теме");
        return;
    }
    
    startNewTest();
}

void AppController::handleAnswerSubmitted(int answerIndex) {
    const Topic* topic = m_courseModel->getTopic(m_currentTopicIndex);
    if (!topic || m_currentQuestionIndex >= topic->questions.size()) {
        return;
    }
    
    // Сохраняем ответ пользователя
    m_userAnswers.append(answerIndex);
    
    // Проверяем правильность ответа
    const Question& question = topic->questions[m_currentQuestionIndex];
    if (answerIndex == question.correctIndex) {
        m_correctAnswers++;
    }
    
    m_currentQuestionIndex++;
    showNextQuestion();
}

void AppController::handleShowProfile() {
    User currentUser = m_sessionManager.getCurrentUser();
    if (!currentUser.isValid()) {
        emit errorOccurred("Пользователь не авторизован");
        return;
    }
    
    // Загружаем результаты пользователя
    if (m_testResultsModel->loadUserResults(currentUser.id)) {
        m_profileWidget->setUser(currentUser);
        m_profileWidget->setResultsModel(m_testResultsModel);
        switchToView(m_profileWidget);
    } else {
        emit errorOccurred("Не удалось загрузить результаты тестов");
    }
}



void AppController::handleAdminBack() {
    switchToView(m_topicWidget);
}

void AppController::handleTestTimeout() {
    finishTest();
    emit showMessage("Время истекло", "Время тестирования истекло. Тест завершен автоматически.");
}

void AppController::handleProgressUpdate(int topicIndex) {
    User currentUser = m_sessionManager.getCurrentUser();
    if (currentUser.isValid() && !currentUser.isAdmin()) {
        saveStudentProgress(currentUser.id, topicIndex);
    }
}

void AppController::handleCourseModelError(const QString& errorMessage) {
    emit errorOccurred(QString("Ошибка модели курса: %1").arg(errorMessage));
}

void AppController::handleTestResultsError(const QString& errorMessage) {
    emit errorOccurred(QString("Ошибка результатов тестов: %1").arg(errorMessage));
}

void AppController::switchToView(QWidget* widget) {
    if (widget && m_stackedWidget) {
        m_stackedWidget->setCurrentWidget(widget);
    }
}

void AppController::startNewTest() {
    const Topic* topic = m_courseModel->getTopic(m_currentTopicIndex);
    if (!topic) {
        return;
    }
    
    // Сброс состояния теста
    m_currentQuestionIndex = 0;
    m_correctAnswers = 0;
    m_userAnswers.clear();
    
    // Запуск таймера (5 минут)
    m_testTimer->start(TEST_TIME_LIMIT_MS);
    
    // Показываем первый вопрос
    showNextQuestion();
    switchToView(m_testWidget);
}

void AppController::finishTest() {
    m_testTimer->stop();
    
    const Topic* topic = m_courseModel->getTopic(m_currentTopicIndex);
    if (!topic) {
        return;
    }
    
    // Сохраняем результат в базу данных
    User currentUser = m_sessionManager.getCurrentUser();
    if (currentUser.isValid()) {
        m_testResultsModel->saveTestResult(currentUser.id, m_correctAnswers, topic->questions.size());
    }
    
    // Показываем результат
    QString message = QString("Тест завершен!\nПравильных ответов: %1 из %2\nПроцент: %3%")
        .arg(m_correctAnswers)
        .arg(topic->questions.size())
        .arg(qRound((double)m_correctAnswers / topic->questions.size() * 100));
    
    emit showMessage("Результат теста", message);
    switchToView(m_topicWidget);
}

void AppController::showNextQuestion() {
    const Topic* topic = m_courseModel->getTopic(m_currentTopicIndex);
    if (!topic) {
        return;
    }
    
    if (m_currentQuestionIndex >= topic->questions.size()) {
        // Все вопросы отвечены
        finishTest();
        return;
    }
    
    const Question& question = topic->questions[m_currentQuestionIndex];
    m_testWidget->showQuestion(question);
}

bool AppController::saveStudentProgress(int userId, int topicId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    
    // Используем INSERT ... ON CONFLICT для PostgreSQL
    query.prepare(R"(
        INSERT INTO progress (user_id, last_topic_id, updated_at) 
        VALUES (?, ?, CURRENT_TIMESTAMP)
        ON CONFLICT (user_id) 
        DO UPDATE SET last_topic_id = EXCLUDED.last_topic_id, updated_at = CURRENT_TIMESTAMP
    )");
    
    query.addBindValue(userId);
    query.addBindValue(topicId);
    
    if (!query.exec()) {
        qWarning() << "Failed to save student progress:" << query.lastError().text();
        return false;
    }
    
    return true;
}

int AppController::loadStudentProgress(int userId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        return -1;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT last_topic_id FROM progress WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qWarning() << "Failed to load student progress:" << query.lastError().text();
        return -1;
    }
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    
    return -1; // Прогресс не найден
}