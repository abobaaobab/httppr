#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QMutex>
#include "DatabaseConfig.h"

/**
 * @brief Singleton класс для управления подключением к базе данных PostgreSQL.
 * 
 * Обеспечивает единое подключение к БД и инициализацию схемы.
 * Использует драйвер QPSQL для работы с PostgreSQL.
 */
class DatabaseManager {
public:
    /**
     * @brief Получить единственный экземпляр DatabaseManager.
     * @return Ссылка на экземпляр DatabaseManager.
     */
    static DatabaseManager& instance();

    /**
     * @brief Установить соединение с базой данных.
     * @return true, если соединение установлено успешно.
     */
    bool connectDb();

    /**
     * @brief Инициализировать схему базы данных.
     * Создает необходимые таблицы и заполняет их начальными данными.
     * @return true, если инициализация прошла успешно.
     */
    bool initSchema();

    /**
     * @brief Проверить, установлено ли соединение с БД.
     * @return true, если соединение активно.
     */
    bool isConnected() const;

    /**
     * @brief Получить объект базы данных для выполнения запросов.
     * @return Ссылка на QSqlDatabase.
     */
    QSqlDatabase& database();

    /**
     * @brief Деструктор. Закрывает соединение с БД.
     */
    ~DatabaseManager();

private:
    /**
     * @brief Приватный конструктор для реализации паттерна Singleton.
     */
    DatabaseManager();

    /**
     * @brief Запретить копирование.
     */
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    /**
     * @brief Создать таблицы в базе данных.
     * @return true, если таблицы созданы успешно.
     */
    bool createTables();

    /**
     * @brief Заполнить таблицы начальными данными.
     * @return true, если данные добавлены успешно.
     */
    bool seedData();

    /**
     * @brief Вычислить SHA-256 хэш строки.
     * @param input Входная строка.
     * @return SHA-256 хэш в виде hex-строки.
     */
    QString calculateSha256(const QString& input) const;

    QSqlDatabase m_database;    ///< Объект подключения к БД
    QMutex m_mutex;            ///< Мьютекс для потокобезопасности
    bool m_connected;          ///< Флаг состояния подключения
};