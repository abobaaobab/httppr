#include "StudentProfileWidget.h"
#include "TestResultDao.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFont>
#include <QDateTime>

StudentProfileWidget::StudentProfileWidget(QWidget *parent)
    : QWidget(parent)
    , m_testHistoryModel(nullptr)
{
    setWindowTitle("Профиль студента");
    
    // Основной layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Заголовок
    QLabel* titleLabel = new QLabel("Профиль студента", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Кнопки управления
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_backButton = new QPushButton("← Назад", this);
    m_refreshButton = new QPushButton("🔄 Обновить", this);
    
    buttonLayout->addWidget(m_backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_refreshButton);
    
    // Добавление секций
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(createUserInfoSection());
    mainLayout->addWidget(createStatisticsSection());
    mainLayout->addWidget(createTestHistorySection(), 1); // Растягиваемая секция
    
    // Подключение сигналов
    connect(m_backButton, &QPushButton::clicked, this, &StudentProfileWidget::onBackClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &StudentProfileWidget::onRefreshClicked);
    
    // Инициализация модели
    m_testHistoryModel = new QSqlQueryModel(this);
}

void StudentProfileWidget::setCurrentUser(const User& user) {
    m_currentUser = user;
    refreshData();
}

void StudentProfileWidget::setUser(const User& user) {
    // Алиас для setCurrentUser для совместимости
    setCurrentUser(user);
}

void StudentProfileWidget::setResultsModel(QAbstractItemModel* model) {
    if (m_testHistoryTable) {
        m_testHistoryTable->setModel(model);
    }
}

void StudentProfileWidget::refreshData() {
    if (!m_currentUser.isValid()) {
        return;
    }
    
    updateUserInfo();
    updateStatistics();
    updateTestHistory();
}

void StudentProfileWidget::onBackClicked() {
    emit backRequested();
}

void StudentProfileWidget::onRefreshClicked() {
    refreshData();
    QMessageBox::information(this, "Обновлено", "Данные профиля обновлены.");
}

QWidget* StudentProfileWidget::createUserInfoSection() {
    QGroupBox* groupBox = new QGroupBox("Информация о пользователе", this);
    QGridLayout* layout = new QGridLayout(groupBox);
    
    // Создание меток
    layout->addWidget(new QLabel("Полное имя:"), 0, 0);
    m_userNameLabel = new QLabel("—", groupBox);
    layout->addWidget(m_userNameLabel, 0, 1);
    
    layout->addWidget(new QLabel("Логин:"), 1, 0);
    m_userLoginLabel = new QLabel("—", groupBox);
    layout->addWidget(m_userLoginLabel, 1, 1);
    
    layout->addWidget(new QLabel("Роль:"), 2, 0);
    m_userRoleLabel = new QLabel("—", groupBox);
    layout->addWidget(m_userRoleLabel, 2, 1);
    
    // Растягивание колонок
    layout->setColumnStretch(1, 1);
    
    return groupBox;
}

QWidget* StudentProfileWidget::createStatisticsSection() {
    QGroupBox* groupBox = new QGroupBox("Статистика тестирования", this);
    QGridLayout* layout = new QGridLayout(groupBox);
    
    // Создание меток статистики
    layout->addWidget(new QLabel("Всего тестов:"), 0, 0);
    m_totalTestsLabel = new QLabel("—", groupBox);
    layout->addWidget(m_totalTestsLabel, 0, 1);
    
    layout->addWidget(new QLabel("Средний балл:"), 0, 2);
    m_averageScoreLabel = new QLabel("—", groupBox);
    layout->addWidget(m_averageScoreLabel, 0, 3);
    
    layout->addWidget(new QLabel("Лучший результат:"), 1, 0);
    m_bestScoreLabel = new QLabel("—", groupBox);
    layout->addWidget(m_bestScoreLabel, 1, 1);
    
    layout->addWidget(new QLabel("Последний тест:"), 1, 2);
    m_lastTestLabel = new QLabel("—", groupBox);
    layout->addWidget(m_lastTestLabel, 1, 3);
    
    // Растягивание колонок
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(3, 1);
    
    return groupBox;
}

QWidget* StudentProfileWidget::createTestHistorySection() {
    QGroupBox* groupBox = new QGroupBox("История тестирования", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    
    // Создание таблицы
    m_testHistoryTable = new QTableView(groupBox);
    m_testHistoryTable->setAlternatingRowColors(true);
    m_testHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_testHistoryTable->setSortingEnabled(true);
    
    // Настройка заголовков
    m_testHistoryTable->horizontalHeader()->setStretchLastSection(true);
    m_testHistoryTable->verticalHeader()->setVisible(false);
    
    layout->addWidget(m_testHistoryTable);
    
    return groupBox;
}

void StudentProfileWidget::updateUserInfo() {
    if (!m_currentUser.isValid()) {
        m_userNameLabel->setText("—");
        m_userLoginLabel->setText("—");
        m_userRoleLabel->setText("—");
        return;
    }
    
    m_userNameLabel->setText(m_currentUser.fullName);
    m_userLoginLabel->setText(m_currentUser.login);
    
    QString roleText = m_currentUser.role;
    if (roleText == "admin") {
        roleText = "Администратор";
    } else if (roleText == "student") {
        roleText = "Студент";
    }
    m_userRoleLabel->setText(roleText);
}

void StudentProfileWidget::updateStatistics() {
    if (!m_currentUser.isValid()) {
        m_totalTestsLabel->setText("—");
        m_averageScoreLabel->setText("—");
        m_bestScoreLabel->setText("—");
        m_lastTestLabel->setText("—");
        return;
    }
    
    // Получение статистики через DAO
    QList<TestResult> results = TestResultDao::findByUserId(m_currentUser.id);
    int totalTests = results.size();
    
    m_totalTestsLabel->setText(QString::number(totalTests));
    
    if (totalTests > 0) {
        // Вычисление средней оценки
        double avgPercentage = TestResultDao::getAverageScore(m_currentUser.id);
        
        // Поиск лучшего результата
        TestResult bestResult = TestResultDao::getBestResult(m_currentUser.id);
        double bestPercentage = bestResult.getPercentage();
        
        // Последний тест (первый в списке, так как отсортирован по дате DESC)
        QDateTime lastTestDate = results.first().testDate;
        
        m_averageScoreLabel->setText(QString("%1%").arg(avgPercentage, 0, 'f', 1));
        m_bestScoreLabel->setText(QString("%1%").arg(bestPercentage, 0, 'f', 1));
        m_lastTestLabel->setText(lastTestDate.toString("dd.MM.yyyy hh:mm"));
    } else {
        m_averageScoreLabel->setText("—");
        m_bestScoreLabel->setText("—");
        m_lastTestLabel->setText("—");
    }
}

void StudentProfileWidget::updateTestHistory() {
    if (!m_currentUser.isValid() || !DatabaseManager::instance().isConnected()) {
        m_testHistoryModel->clear();
        return;
    }
    
    // SQL запрос для истории тестов
    QString queryString = 
        "SELECT "
            "id as \"ID\", "
            "test_date as \"Дата и время\", "
            "score as \"Набрано баллов\", "
            "max_score as \"Максимум баллов\", "
            "ROUND(CAST(score AS FLOAT) / CAST(max_score AS FLOAT) * 100, 1) as \"Процент (%)\" "
        "FROM test_results "
        "WHERE user_id = ? "
        "ORDER BY test_date DESC";
    
    // Подготовка запроса с параметром
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(queryString);
    query.addBindValue(m_currentUser.id);
    
    if (!query.exec()) {
        qCritical() << "Failed to fetch test history:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить историю тестов.");
        return;
    }
    
    // Установка модели
    m_testHistoryModel->setQuery(query);
    m_testHistoryTable->setModel(m_testHistoryModel);
    
    // Проверка на ошибки модели
    if (m_testHistoryModel->lastError().isValid()) {
        qCritical() << "Model error:" << m_testHistoryModel->lastError().text();
        QMessageBox::warning(this, "Ошибка", "Ошибка при отображении данных.");
        return;
    }
    
    // Настройка размеров колонок
    m_testHistoryTable->resizeColumnsToContents();
    
    qDebug() << "Test history updated for user" << m_currentUser.login 
             << "- found" << m_testHistoryModel->rowCount() << "records";
}