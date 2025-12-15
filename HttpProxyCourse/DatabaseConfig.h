#pragma once

#include <QString>
#include <QSettings>

/*!
 * @brief Класс для управления конфигурацией базы данных.
 * 
 * Обеспечивает централизованное управление параметрами подключения к БД
 * с возможностью загрузки из файла конфигурации.
 */
class DatabaseConfig {
public:
    /*!
     * @brief Получить единственный экземпляр DatabaseConfig.
     * @return Ссылка на экземпляр DatabaseConfig.
     */
    static DatabaseConfig& instance();

    /*!
     * @brief Загрузить конфигурацию из файла.
     * @param configPath Путь к файлу конфигурации (по умолчанию "config.ini").
     */
    void loadConfig(const QString& configPath = "config.ini");

    /*!
     * @brief Получить имя хоста БД.
     * @return Имя хоста.
     */
    QString hostName() const { return m_hostName; }

    /*!
     * @brief Получить имя базы данных.
     * @return Имя базы данных.
     */
    QString databaseName() const { return m_databaseName; }

    /*!
     * @brief Получить имя пользователя БД.
     * @return Имя пользователя.
     */
    QString userName() const { return m_userName; }

    /*!
     * @brief Получить пароль пользователя БД.
     * @return Пароль пользователя.
     */
    QString password() const { return m_password; }

    /*!
     * @brief Получить порт БД.
     * @return Номер порта.
     */
    int port() const { return m_port; }

private:
    /*!
     * @brief Приватный конструктор для реализации паттерна Singleton.
     */
    DatabaseConfig();

    /*!
     * @brief Запретить копирование.
     */
    DatabaseConfig(const DatabaseConfig&) = delete;
    DatabaseConfig& operator=(const DatabaseConfig&) = delete;

    /*!
     * @brief Создать файл конфигурации по умолчанию.
     * @param configPath Путь к файлу конфигурации.
     */
    void createDefaultConfig(const QString& configPath);

    QString m_hostName;      ///< Имя хоста БД
    QString m_databaseName;  ///< Имя базы данных
    QString m_userName;      ///< Имя пользователя БД
    QString m_password;      ///< Пароль пользователя БД
    int m_port;              ///< Порт БД
};