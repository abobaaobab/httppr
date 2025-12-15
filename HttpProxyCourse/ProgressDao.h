#pragma once

#include <QString>
#include <QDateTime>

/**
 * @brief Структура для хранения прогресса пользователя.
 */
struct UserProgress {
    int userId;              ///< ID пользователя
    int lastTopicId;         ///< ID последней изученной темы
    QDateTime updatedAt;     ///< Время последнего обновления
    
    /**
     * @brief Конструктор по умолчанию.
     */
    UserProgress() : userId(-1), lastTopicId(-1) {}
    
    /**
     * @brief Конструктор с параметрами.
     */
    UserProgress(int uid, int topicId, const QDateTime& updated = QDateTime::currentDateTime())
        : userId(uid), lastTopicId(topicId), updatedAt(updated) {}
};

/**
 * @brief Data Access Object для работы с прогрессом пользователей.
 * 
 * Управляет таблицей progress, отслеживающей последнюю изученную тему
 * каждого пользователя.
 */
class ProgressDao {
public:
    /**
     * @brief Получить прогресс пользователя.
     * @param userId ID пользователя.
     * @return Объект UserProgress или невалидный объект, если не найден.
     */
    static UserProgress findByUserId(int userId);

    /**
     * @brief Обновить прогресс пользователя.
     * @param userId ID пользователя.
     * @param topicId ID последней изученной темы.
     * @return true, если обновление прошло успешно.
     */
    static bool updateProgress(int userId, int topicId);

    /**
     * @brief Создать запись о прогрессе для нового пользователя.
     * @param userId ID пользователя.
     * @param topicId Начальная тема (обычно 0).
     * @return true, если создание прошло успешно.
     */
    static bool createProgress(int userId, int topicId = 0);

    /**
     * @brief Удалить прогресс пользователя.
     * @param userId ID пользователя.
     * @return true, если удаление прошло успешно.
     */
    static bool deleteByUserId(int userId);
};