#pragma once

#include "DomainTypes.h"
#include "Serializer.h"
#include <QString>
#include <QList>
#include <QDateTime>

/**
 * @brief Менеджер сессии обучения.
 * 
 * Управляет состоянием текущей сессии обучения, включая загрузку курса,
 * навигацию по темам и вопросам, обработку ответов студента.
 */
class SessionManager {
public:
    /// @brief Максимальное количество ошибок в теме перед принудительным повтором
    static const int MAX_ERRORS = 3;
    /**
     * @brief Результат отправки ответа на вопрос.
     */
    enum class SubmitResult {
        Correct,        ///< Правильный ответ
        Wrong,          ///< Неправильный ответ
        FailRelearn,    ///< Слишком много ошибок, нужно повторить тему
        TopicFinished,  ///< Тема завершена успешно
        CourseFinished  ///< Весь курс завершен
    };

    /**
     * @brief Конструктор менеджера сессии.
     */
    SessionManager();

    /**
     * @brief Устанавливает текущего пользователя сессии.
     * @param user Пользователь для установки.
     */
    void setCurrentUser(const User& user);

    /**
     * @brief Возвращает текущего пользователя.
     * @return Текущий пользователь или невалидный User, если не установлен.
     */
    const User& getCurrentUser() const;

    /**
     * @brief Проверяет, установлен ли пользователь.
     * @return true, если пользователь установлен.
     */
    bool hasUser() const;

    /**
     * @brief Очищает текущую сессию пользователя.
     */
    void clearSession();

    /**
     * @brief Загружает курс из файла.
     * @param filePath Путь к файлу с данными курса.
     */
    void loadCourse(const QString& filePath);

    /**
     * @brief Проверяет, загружен ли курс.
     * @return true, если курс загружен.
     */
    bool isCourseLoaded() const;

    /**
     * @brief Начинает изучение указанной темы.
     * @param topicIndex Индекс темы для изучения.
     */
    void startTopic(int topicIndex);

    /**
     * @brief Возвращает константную ссылку на курс.
     * @return Ссылка на объект курса.
     */
    const Course& getCourse() const;

    /**
     * @brief Возвращает изменяемую ссылку на курс.
     * @return Изменяемая ссылка на объект курса.
     */
    Course& getMutableCourse();

    /**
     * @brief Возвращает указатель на текущую тему.
     * @return Указатель на текущую тему или nullptr.
     */
    Topic* getCurrentTopic();

    /**
     * @brief Возвращает указатель на текущий вопрос.
     * @return Указатель на текущий вопрос или nullptr.
     */
    Question* getCurrentQuestion();

    /**
     * @brief Обрабатывает ответ студента на текущий вопрос.
     * @param answerIndex Индекс выбранного ответа.
     * @return Результат обработки ответа.
     */
    SubmitResult submitAnswer(int answerIndex);

    /**
     * @brief Сохраняет прогресс пользователя в базу данных.
     * @return true, если прогресс сохранен успешно.
     */
    bool saveProgress();

    /**
     * @brief Загружает прогресс пользователя из базы данных.
     * @return true, если прогресс загружен успешно.
     */
    bool loadProgress();

private:
    Course currentCourse;
    bool m_isLoaded;
    User m_currentUser;

    qint32 currentTopicIndex;
    qint32 currentQuestionIndex;
    qint32 errorsInTopic;
};
