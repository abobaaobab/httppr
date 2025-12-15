#include "SessionManager.h"
#include "DatabaseManager.h"
#include <stdexcept>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SessionManager::SessionManager()
    : m_isLoaded(false)
    , currentTopicIndex(-1)
    , currentQuestionIndex(-1)
    , errorsInTopic(0)
{
}

void SessionManager::loadCourse(const QString& filePath) {
    try {
        currentCourse = Serializer::load(filePath);
        m_isLoaded = true;
        currentTopicIndex = -1;
        currentQuestionIndex = -1;
        errorsInTopic = 0;
    } catch (const std::exception& e) {
        m_isLoaded = false;
        throw std::runtime_error(std::string("Failed to load course: ") + e.what());
    }
}

bool SessionManager::isCourseLoaded() const {
    return m_isLoaded;
}

void SessionManager::startTopic(int topicIndex) {
    if (!m_isLoaded) {
        throw std::runtime_error("Course not loaded");
    }
    // Безопасное сравнение индексов
    if (topicIndex < 0 || topicIndex >= currentCourse.topics.size()) {
        throw std::runtime_error("Invalid topic index");
    }

    currentTopicIndex = topicIndex;
    currentQuestionIndex = 0;
    errorsInTopic = 0;
}

const Course& SessionManager::getCourse() const {
    return currentCourse;
}

Course& SessionManager::getMutableCourse() {
    return currentCourse;
}

Topic* SessionManager::getCurrentTopic() {
    if (!m_isLoaded || currentTopicIndex < 0 || currentTopicIndex >= currentCourse.topics.size()) {
        return nullptr;
    }
    return &currentCourse.topics[currentTopicIndex];
}

Question* SessionManager::getCurrentQuestion() {
    Topic* topic = getCurrentTopic();
    if (!topic) return nullptr;

    if (currentQuestionIndex < 0 || currentQuestionIndex >= topic->questions.size()) {
        return nullptr;
    }
    return &topic->questions[currentQuestionIndex];
}

SessionManager::SubmitResult SessionManager::submitAnswer(int answerIndex) {
    // Критическая проверка: курс должен быть загружен
    if (!m_isLoaded) {
        throw std::runtime_error("Cannot submit answer: course not loaded");
    }

    Topic* topic = getCurrentTopic();
    Question* question = getCurrentQuestion();

    if (!topic || !question) {
        throw std::runtime_error("Cannot submit answer: invalid topic or question state");
    }

    // Проверяем границы answerIndex
    if (answerIndex < 0 || answerIndex >= question->variants.size()) {
        return SubmitResult::Wrong;
    }

    // Проверяем корректность correctIndex (дополнительная защита)
    if (question->correctIndex < 0 || question->correctIndex >= question->variants.size()) {
        return SubmitResult::Wrong;
    }

    if (answerIndex == question->correctIndex) {
        currentQuestionIndex++;

        if (currentQuestionIndex >= topic->questions.size()) {
            if (currentTopicIndex >= currentCourse.topics.size() - 1) {
                return SubmitResult::CourseFinished;
            }
            return SubmitResult::TopicFinished;
        }
        return SubmitResult::Correct;
    } else {
        errorsInTopic++;
        if (errorsInTopic >= MAX_ERRORS) {
            currentQuestionIndex = 0;
            errorsInTopic = 0;
            return SubmitResult::FailRelearn;
        }
        return SubmitResult::Wrong;
    }
}

void SessionManager::setCurrentUser(const User& user) {
    m_currentUser = user;
    if (user.isValid()) {
        loadProgress();
    }
}

const User& SessionManager::getCurrentUser() const {
    return m_currentUser;
}

bool SessionManager::hasUser() const {
    return m_currentUser.isValid();
}

void SessionManager::clearSession() {
    m_currentUser = User(); // Сброс пользователя
    currentTopicIndex = -1;
    currentQuestionIndex = -1;
    errorsInTopic = 0;
}

bool SessionManager::saveProgress() {
    if (!hasUser() || !DatabaseManager::instance().isConnected()) {
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO progress (user_id, last_topic_id, updated_at) 
        VALUES (?, ?, CURRENT_TIMESTAMP)
        ON CONFLICT (user_id) 
        DO UPDATE SET last_topic_id = ?, updated_at = CURRENT_TIMESTAMP
    )");
    
    query.addBindValue(m_currentUser.id);
    query.addBindValue(currentTopicIndex);
    query.addBindValue(currentTopicIndex);

    if (!query.exec()) {
        qCritical() << "Failed to save progress:" << query.lastError().text();
        return false;
    }

    qDebug() << "Progress saved for user" << m_currentUser.login << "topic" << currentTopicIndex;
    return true;
}

bool SessionManager::loadProgress() {
    if (!hasUser() || !DatabaseManager::instance().isConnected()) {
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT last_topic_id FROM progress WHERE user_id = ?");
    query.addBindValue(m_currentUser.id);

    if (!query.exec()) {
        qCritical() << "Failed to load progress:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int lastTopicId = query.value("last_topic_id").toInt();
        qDebug() << "Loaded progress for user" << m_currentUser.login << "last topic" << lastTopicId;
        
        // Устанавливаем прогресс только если курс загружен
        if (m_isLoaded && lastTopicId >= 0 && lastTopicId < currentCourse.topics.size()) {
            currentTopicIndex = lastTopicId;
            currentQuestionIndex = 0;
            errorsInTopic = 0;
        }
        return true;
    }

    qDebug() << "No progress found for user" << m_currentUser.login;
    return false;
}
