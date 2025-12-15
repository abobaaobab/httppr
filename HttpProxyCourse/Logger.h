#pragma once

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

/*!
 * @brief Простая система логирования для приложения.
 * 
 * Обеспечивает структурированное логирование с различными уровнями важности
 * и возможностью записи в файл.
 */
class Logger {
public:
    /*!
     * @brief Уровни логирования.
     */
    enum class Level {
        Debug,    ///< Отладочная информация
        Info,     ///< Информационные сообщения
        Warning,  ///< Предупреждения
        Error,    ///< Ошибки
        Critical  ///< Критические ошибки
    };

    /*!
     * @brief Записать сообщение в лог.
     * @param level Уровень важности сообщения.
     * @param message Текст сообщения.
     * @param category Категория сообщения (например, "Database", "Auth").
     */
    static void log(Level level, const QString& message, const QString& category = "General");

    /*!
     * @brief Включить/выключить запись в файл.
     * @param enabled true для включения записи в файл.
     * @param filePath Путь к файлу лога (по умолчанию "application.log").
     */
    static void setFileLogging(bool enabled, const QString& filePath = "application.log");

    // Удобные методы для различных уровней логирования
    static void debug(const QString& message, const QString& category = "General") {
        log(Level::Debug, message, category);
    }
    
    static void info(const QString& message, const QString& category = "General") {
        log(Level::Info, message, category);
    }
    
    static void warning(const QString& message, const QString& category = "General") {
        log(Level::Warning, message, category);
    }
    
    static void error(const QString& message, const QString& category = "General") {
        log(Level::Error, message, category);
    }
    
    static void critical(const QString& message, const QString& category = "General") {
        log(Level::Critical, message, category);
    }

private:
    /*!
     * @brief Преобразовать уровень логирования в строку.
     * @param level Уровень логирования.
     * @return Строковое представление уровня.
     */
    static QString levelToString(Level level);

    static bool s_fileLoggingEnabled;  ///< Флаг записи в файл
    static QString s_logFilePath;      ///< Путь к файлу лога
};