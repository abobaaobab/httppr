#pragma once

#include <QString>
#include <QDateTime>
#include <QList>

/**
 * @brief Структура для хранения результата тестирования.
 */
struct TestResult {
    int id;                  ///< ID записи
    int userId;              ///< ID пользователя
    QDateTime testDate;      ///< Дата прохождения теста
    int score;               ///< Набранные баллы
    int maxScore;            ///< Максимальные баллы
    
    /**
     * @brief Конструктор по умолчанию.
     */
    TestResult() : id(-1), userId(-1), score(0), maxScore(0) {}
    
    /**
     * @brief Конструктор с параметрами.
     */
    TestResult(int uid, int userScore, int maxUserScore, const QDateTime& date = QDateTime::currentDateTime())
        : id(-1), userId(uid), testDate(date), score(userScore), maxScore(maxUserScore) {}
    
    /**
     * @brief Вычислить процент правильных ответов.
     * @return Процент от 0 до 100.
     */
    double getPercentage() const {
        return maxScore > 0 ? (static_cast<double>(score) / maxScore) * 100.0 : 0.0;
    }
};

/**
 * @brief Data Access Object для работы с результатами тестирования.
 * 
 * Управляет таблицей test_results, хранящей историю прохождения тестов
 * всеми пользователями.
 */
class TestResultDao {
public:
    /**
     * @brief Сохранить результат тестирования.
     * @param result Результат для сохранения.
     * @return true, если сохранение прошло успешно.
     */
    static bool save(const TestResult& result);

    /**
     * @brief Получить все результаты пользователя.
     * @param userId ID пользователя.
     * @return Список результатов, отсортированный по дате (новые первыми).
     */
    static QList<TestResult> findByUserId(int userId);

    /**
     * @brief Получить все результаты всех пользователей (для админки).
     * @return Список всех результатов.
     */
    static QList<TestResult> findAll();

    /**
     * @brief Получить лучший результат пользователя.
     * @param userId ID пользователя.
     * @return Лучший результат или невалидный объект.
     */
    static TestResult getBestResult(int userId);

    /**
     * @brief Получить средний балл пользователя.
     * @param userId ID пользователя.
     * @return Средний процент правильных ответов.
     */
    static double getAverageScore(int userId);

    /**
     * @brief Удалить все результаты пользователя.
     * @param userId ID пользователя.
     * @return true, если удаление прошло успешно.
     */
    static bool deleteByUserId(int userId);
};