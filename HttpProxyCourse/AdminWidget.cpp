#include "AdminWidget.h"
#include "UserDao.h"
#include "TestResultDao.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>

// Константа для имени файла курса
static const QString COURSE_DATA_FILE = "course.dat";

AdminWidget::AdminWidget(Course* course, QWidget* parent)
    : QWidget(parent)
    , m_course(course)
    , m_statisticsModel(nullptr)
    , m_proxyModel(nullptr)
{
    // Критическая проверка: курс не может быть nullptr
    if (!course) {
        QMessageBox::critical(this, "Критическая ошибка", 
                            "Не удалось инициализировать панель администратора: отсутствуют данные курса");
        // Блокируем интерфейс при отсутствии данных
        setEnabled(false);
        return;
    }
    
    setupUi();
    loadTopics();
}

void AdminWidget::setCurrentUser(const User& user) {
    m_currentUser = user;
    setupAccessRights();
}

void AdminWidget::setupUi() {
    m_layout = new QVBoxLayout(this);

    // Заголовок
    QLabel* titleLabel = new QLabel("Панель Администратора", this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(titleLabel);

    // Создание вкладок
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createCourseEditTab(), "Редактирование курса");
    m_tabWidget->addTab(createStudentStatisticsTab(), "Статистика студентов");
    
    m_layout->addWidget(m_tabWidget);

    // Кнопка выхода
    m_btnLogout = new QPushButton("← Выйти", this);
    m_layout->addWidget(m_btnLogout);

    // Подключение сигналов
    connect(m_btnLogout, &QPushButton::clicked, this, &AdminWidget::onLogoutClicked);
}

QWidget* AdminWidget::createCourseEditTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    m_lblHeader = new QLabel("Редактор Лекций", tab);
    QFont font = m_lblHeader->font();
    font.setBold(true);
    font.setPointSize(12);
    m_lblHeader->setFont(font);
    m_lblHeader->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_lblHeader);

    layout->addWidget(new QLabel("Выберите тему для редактирования:", tab));
    m_cbTopics = new QComboBox(tab);
    layout->addWidget(m_cbTopics);

    layout->addWidget(new QLabel("HTML Контент (QTextBrowser совместимый):", tab));
    m_txtHtmlEditor = new QTextEdit(tab);
    m_txtHtmlEditor->setAcceptRichText(false);
    layout->addWidget(m_txtHtmlEditor);

    m_btnSave = new QPushButton("Сохранить изменения в файл", tab);
    layout->addWidget(m_btnSave);

    // Подключение сигналов
    connect(m_cbTopics, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdminWidget::onTopicChanged);
    connect(m_btnSave, &QPushButton::clicked, this, &AdminWidget::onSaveClicked);

    return tab;
}

QWidget* AdminWidget::createStudentStatisticsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    // Заголовок секции
    QLabel* headerLabel = new QLabel("Статистика студентов", tab);
    QFont headerFont = headerLabel->font();
    headerFont.setBold(true);
    headerFont.setPointSize(12);
    headerLabel->setFont(headerFont);
    headerLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(headerLabel);

    // Панель фильтрации
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Поиск по имени:", tab));
    
    m_filterEdit = new QLineEdit(tab);
    m_filterEdit->setPlaceholderText("Введите имя студента для поиска...");
    filterLayout->addWidget(m_filterEdit);
    
    m_btnRefreshStats = new QPushButton("🔄 Обновить", tab);
    filterLayout->addWidget(m_btnRefreshStats);
    
    layout->addLayout(filterLayout);

    // Таблица статистики
    m_statisticsTable = new QTableView(tab);
    m_statisticsTable->setAlternatingRowColors(true);
    m_statisticsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_statisticsTable->setSortingEnabled(true);
    m_statisticsTable->horizontalHeader()->setStretchLastSection(true);
    m_statisticsTable->verticalHeader()->setVisible(false);
    
    layout->addWidget(m_statisticsTable);

    // Инициализация моделей
    m_statisticsModel = new QSqlQueryModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_statisticsModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(1); // Фильтр по колонке "Полное имя"
    
    m_statisticsTable->setModel(m_proxyModel);

    // Подключение сигналов
    connect(m_filterEdit, &QLineEdit::textChanged, this, &AdminWidget::onFilterChanged);
    connect(m_btnRefreshStats, &QPushButton::clicked, this, &AdminWidget::onRefreshStatistics);

    // Загрузка данных
    updateStudentStatistics();

    return tab;
}

void AdminWidget::loadTopics() {
    if (!m_course) return;

    m_cbTopics->blockSignals(true);
    m_cbTopics->clear();

    for (const Topic& topic : m_course->topics) {
        m_cbTopics->addItem(topic.title);
    }

    m_cbTopics->blockSignals(false);

    if (m_cbTopics->count() > 0) {
        m_cbTopics->setCurrentIndex(0);
        onTopicChanged(0);
    }
}

void AdminWidget::onTopicChanged(int index) {
    if (!m_course || index < 0 || index >= m_course->topics.size()) {
        m_txtHtmlEditor->clear();
        return;
    }

    m_txtHtmlEditor->setPlainText(m_course->topics[index].htmlContent);
}

void AdminWidget::onSaveClicked() {
    if (!m_course) return;

    int index = m_cbTopics->currentIndex();
    if (index < 0 || index >= m_course->topics.size()) {
        QMessageBox::warning(this, "Ошибка", "Нет выбранной темы для сохранения.");
        return;
    }

    try {
        QString newContent = m_txtHtmlEditor->toPlainText();
        if (newContent.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", "Текст лекции пуст!");
            return;
        }
        m_course->topics[index].htmlContent = newContent;

        Serializer::save(*m_course, COURSE_DATA_FILE);
        QMessageBox::information(this, "Успех", "Курс успешно сохранен и зашифрован!");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка сохранения",
                              QString("Не удалось записать файл:\n%1").arg(e.what()));
    }
}

void AdminWidget::onLogoutClicked() {
    if (m_txtHtmlEditor) {
        m_txtHtmlEditor->clear();
    }
    emit backRequested();
}

void AdminWidget::onFilterChanged() {
    if (m_proxyModel) {
        m_proxyModel->setFilterFixedString(m_filterEdit->text());
    }
}

void AdminWidget::onRefreshStatistics() {
    updateStudentStatistics();
    QMessageBox::information(this, "Обновлено", "Статистика студентов обновлена.");
}

void AdminWidget::updateStudentStatistics() {
    if (!DatabaseManager::instance().isConnected()) {
        qWarning() << "Database not connected, cannot update student statistics";
        return;
    }

    // SQL запрос для получения статистики всех студентов с фильтрацией по фамилии
    QString queryString = 
        "SELECT "
            "u.id as \"ID\", "
            "u.full_name as \"Полное имя\", "
            "u.login as \"Логин\", "
            "COALESCE(COUNT(tr.id), 0) as \"Всего тестов\", "
            "COALESCE(ROUND(AVG(CAST(tr.score AS FLOAT) / CAST(tr.max_score AS FLOAT) * 100), 1), 0) as \"Средний балл (%)\", "
            "COALESCE(MAX(CAST(tr.score AS FLOAT) / CAST(tr.max_score AS FLOAT) * 100), 0) as \"Лучший результат (%)\", "
            "COALESCE(MAX(tr.test_date), 'Нет данных') as \"Последний тест\", "
            "COALESCE(p.last_topic_id, 0) as \"Последняя тема\" "
        "FROM users u "
        "LEFT JOIN test_results tr ON u.id = tr.user_id "
        "LEFT JOIN progress p ON u.id = p.user_id "
        "WHERE u.role = 'student' "
        "GROUP BY u.id, u.full_name, u.login, p.last_topic_id "
        "ORDER BY u.full_name";

    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(queryString)) {
        qCritical() << "Failed to fetch student statistics:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить статистику студентов.");
        return;
    }

    // Установка запроса в модель
    m_statisticsModel->setQuery(query);
    
    if (m_statisticsModel->lastError().isValid()) {
        qCritical() << "Model error:" << m_statisticsModel->lastError().text();
        QMessageBox::warning(this, "Ошибка", "Ошибка при отображении статистики.");
        return;
    }

    // Настройка фильтра по фамилии (QSortFilterProxyModel)
    if (m_proxyModel) {
        m_proxyModel->setFilterKeyColumn(1); // Колонка "Полное имя"
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    // Настройка размеров колонок
    m_statisticsTable->resizeColumnsToContents();
    
    qDebug() << "Student statistics updated - found" << m_statisticsModel->rowCount() << "students";
}

void AdminWidget::setupAccessRights() {
    if (!m_tabWidget) return;
    
    // Проверяем права доступа к вкладке статистики
    bool isAdmin = m_currentUser.isValid() && m_currentUser.isAdmin();
    
    // Скрываем/показываем вкладку статистики в зависимости от роли
    if (m_tabWidget->count() > 1) {
        m_tabWidget->setTabEnabled(1, isAdmin); // Вкладка статистики студентов
        if (!isAdmin) {
            m_tabWidget->setCurrentIndex(0); // Переключаемся на первую вкладку
        }
    }
    
    qDebug() << "Access rights setup for user:" << m_currentUser.login 
             << "Admin access:" << isAdmin;
}
