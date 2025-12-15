#pragma once

#include <QObject>
#include <QString>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include "DomainTypes.h"

/**
 * @brief Модель данных курса (Model в архитектуре MVC).
 * 
 * Отвечает за загрузку, хранение и управление данными курса.
 * Обеспечивает доступ к темам, вопросам и прогрессу студентов.
 */
class CourseModel : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Конструктор модели курса.
     * @param parent Родительский объект для управления памятью.
     */
    explicit CourseModel(QObject* parent = nullptr);

    /**
     * @brief Загружает данные курса из бинарного файла.
     * @param filePath Путь к файлу с данными курса.
     * @return true, если загрузка прошла успешно.
     */
    bool loadCourse(const QString& filePath);

    /**
     * @brief Сохраняет данные курса в бинарный файл.
     * @param filePath Путь для сохранения файла.
     * @return true, если сохранение прошло успешно.
     */
    bool saveCourse(const QString& filePath);

    /**
     * @brief Получить количество тем в курсе.
     * @return Количество тем.
     */
    int getTopicCount() const;

    /**
     * @brief Получить тему по индексу.
     * @param index Индекс темы (0-based).
     * @return Указатель на тему или nullptr, если индекс неверный.
     */
    const Topic* getTopic(int index) const;

    /**
     * @brief Добавить новую тему в курс.
     * @param topic Тема для добавления.
     */
    void addTopic(const Topic& topic);

    /**
     * @brief Обновить существующую тему.
     * @param index Индекс темы для обновления.
     * @param topic Новые данные темы.
     * @return true, если обновление прошло успешно.
     */
    bool updateTopic(int index, const Topic& topic);

    /**
     * @brief Удалить тему по индексу.
     * @param index Индекс темы для удаления.
     * @return true, если удаление прошло успешно.
     */
    bool removeTopic(int index);

    /**
     * @brief Получить список названий всех тем.
     * @return Список названий тем.
     */
    QStringList getTopicTitles() const;

    /**
     * @brief Получить все темы курса.
     * @return Список всех тем.
     */
    const QList<Topic>& getTopics() const;

signals:
    /**
     * @brief Сигнал об изменении данных курса.
     */
    void courseDataChanged();

    /**
     * @brief Сигнал об ошибке при работе с данными.
     * @param errorMessage Сообщение об ошибке.
     */
    void errorOccurred(const QString& errorMessage);

private:
    Course m_course;  ///< Данные курса
};

/**
 * @brief Модель для работы с результатами тестов студентов.
 * 
 * Предоставляет интерфейс для отображения и фильтрации результатов
 * тестирования в табличном виде.
 */
class TestResultsModel : public QSqlQueryModel {
    Q_OBJECT

public:
    /**
     * @brief Конструктор модели результатов тестов.
     * @param parent Родительский объект для управления памятью.
     */
    explicit TestResultsModel(QObject* parent = nullptr);

    /**
     * @brief Загружает результаты тестов для конкретного пользователя.
     * @param userId ID пользователя.
     * @return true, если загрузка прошла успешно.
     */
    bool loadUserResults(int userId);

    /**
     * @brief Загружает результаты всех пользователей (для администратора).
     * @return true, если загрузка прошла успешно.
     */
    bool loadAllResults();

    /**
     * @brief Сохраняет результат теста в базу данных.
     * @param userId ID пользователя.
     * @param score Набранные баллы.
     * @param maxScore Максимальные баллы.
     * @return true, если сохранение прошло успешно.
     */
    bool saveTestResult(int userId, int score, int maxScore);

    // Переопределение для кастомных заголовков
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

signals:
    /**
     * @brief Сигнал об ошибке при работе с БД.
     * @param errorMessage Сообщение об ошибке.
     */
    void databaseError(const QString& errorMessage);
};

/**
 * @brief Прокси-модель для фильтрации результатов тестов.
 * 
 * Позволяет фильтровать результаты по фамилии студента
 * согласно требованиям ТЗ.
 */
class TestResultsFilterModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    /**
     * @brief Конструктор прокси-модели.
     * @param parent Родительский объект для управления памятью.
     */
    explicit TestResultsFilterModel(QObject* parent = nullptr);

    /**
     * @brief Установить фильтр по фамилии студента.
     * @param surname Фамилия для фильтрации (частичное совпадение).
     */
    void setNameFilter(const QString& surname);

protected:
    /**
     * @brief Переопределение фильтрации строк.
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString m_nameFilter;  ///< Текущий фильтр по фамилии
};