#include "CourseModel.h"
#include "Serializer.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ==================== CourseModel ====================

CourseModel::CourseModel(QObject* parent) : QObject(parent) {
    // Инициализация модели
}

bool CourseModel::loadCourse(const QString& filePath) {
    try {
        m_course = Serializer::load(filePath);
        emit courseDataChanged();
        return true;
    } catch (const std::exception& e) {
        QString errorMsg = QString("Ошибка загрузки курса: %1").arg(e.what());
        emit errorOccurred(errorMsg);
        qCritical() << errorMsg;
        return false;
    }
}

bool CourseModel::saveCourse(const QString& filePath) {
    try {
        Serializer::save(m_course, filePath);
        return true;
    } catch (const std::exception& e) {
        QString errorMsg = QString("Ошибка сохранения курса: %1").arg(e.what());
        emit errorOccurred(errorMsg);
        qCritical() << errorMsg;
        return false;
    }
}

int CourseModel::getTopicCount() const {
    return m_course.topics.size();
}

const Topic* CourseModel::getTopic(int index) const {
    if (index < 0 || index >= m_course.topics.size()) {
        return nullptr;
    }
    return &m_course.topics[index];
}

void CourseModel::addTopic(const Topic& topic) {
    m_course.topics.append(topic);
    emit courseDataChanged();
}

bool CourseModel::updateTopic(int index, const Topic& topic) {
    if (index < 0 || index >= m_course.topics.size()) {
        return false;
    }
    m_course.topics[index] = topic;
    emit courseDataChanged();
    return true;
}

bool CourseModel::removeTopic(int index) {
    if (index < 0 || index >= m_course.topics.size()) {
        return false;
    }
    m_course.topics.removeAt(index);
    emit courseDataChanged();
    return true;
}

QStringList CourseModel::getTopicTitles() const {
    QStringList titles;
    for (const Topic& topic : m_course.topics) {
        titles << topic.title;
    }
    return titles;
}

const QList<Topic>& CourseModel::getTopics() const {
    return m_course.topics;
}

// ==================== TestResultsModel ====================

TestResultsModel::TestResultsModel(QObject* parent) : QSqlQueryModel(parent) {
    // Инициализация модели результатов
}

bool TestResultsModel::loadUserResults(int userId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        emit databaseError("База данных не подключена");
        return false;
    }

    QString queryStr = R"(
        SELECT 
            tr.test_date as "Дата тестирования",
            tr.score as "Набрано баллов", 
            tr.max_score as "Максимум баллов",
            ROUND((tr.score::float / tr.max_score::float) * 100, 1) as "Процент"
        FROM test_results tr 
        WHERE tr.user_id = ?
        ORDER BY tr.test_date DESC
    )";

    QSqlQuery query(dbManager.database());
    query.prepare(queryStr);
    query.addBindValue(userId);

    if (!query.exec()) {
        QString errorMsg = QString("Ошибка загрузки результатов: %1").arg(query.lastError().text());
        emit databaseError(errorMsg);
        return false;
    }

    setQuery(query);
    return true;
}

bool TestResultsModel::loadAllResults() {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        emit databaseError("База данных не подключена");
        return false;
    }

    QString queryStr = R"(
        SELECT 
            u.full_name as "ФИО студента",
            tr.test_date as "Дата тестирования",
            tr.score as "Набрано баллов", 
            tr.max_score as "Максимум баллов",
            ROUND((tr.score::float / tr.max_score::float) * 100, 1) as "Процент"
        FROM test_results tr 
        JOIN users u ON tr.user_id = u.id
        WHERE u.role = 'student'
        ORDER BY tr.test_date DESC, u.full_name
    )";

    QSqlQuery query(dbManager.database());
    if (!query.exec(queryStr)) {
        QString errorMsg = QString("Ошибка загрузки всех результатов: %1").arg(query.lastError().text());
        emit databaseError(errorMsg);
        return false;
    }

    setQuery(query);
    return true;
}

bool TestResultsModel::saveTestResult(int userId, int score, int maxScore) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        emit databaseError("База данных не подключена");
        return false;
    }

    QSqlQuery query(dbManager.database());
    query.prepare(R"(
        INSERT INTO test_results (user_id, score, max_score) 
        VALUES (?, ?, ?)
    )");
    
    query.addBindValue(userId);
    query.addBindValue(score);
    query.addBindValue(maxScore);

    if (!query.exec()) {
        QString errorMsg = QString("Ошибка сохранения результата: %1").arg(query.lastError().text());
        emit databaseError(errorMsg);
        return false;
    }

    return true;
}

QVariant TestResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    // Используем стандартные заголовки из запроса
    return QSqlQueryModel::headerData(section, orientation, role);
}

// ==================== TestResultsFilterModel ====================

TestResultsFilterModel::TestResultsFilterModel(QObject* parent) 
    : QSortFilterProxyModel(parent) {
    // Настройка фильтрации по первому столбцу (ФИО)
    setFilterKeyColumn(0);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void TestResultsFilterModel::setNameFilter(const QString& surname) {
    m_nameFilter = surname.trimmed();
    setFilterRegExp(QRegExp(m_nameFilter, Qt::CaseInsensitive, QRegExp::FixedString));
}

bool TestResultsFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    if (m_nameFilter.isEmpty()) {
        return true; // Показываем все строки, если фильтр пуст
    }

    // Получаем данные из первого столбца (ФИО)
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString fullName = sourceModel()->data(index).toString();
    
    // Проверяем, содержит ли ФИО искомую подстроку
    return fullName.contains(m_nameFilter, Qt::CaseInsensitive);
}