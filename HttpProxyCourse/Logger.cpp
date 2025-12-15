#include "Logger.h"
#include <QMutex>
#include <QMutexLocker>

// Статические переменные
bool Logger::s_fileLoggingEnabled = false;
QString Logger::s_logFilePath = "application.log";

// Мьютекс для потокобезопасности
static QMutex s_logMutex;

void Logger::log(Level level, const QString& message, const QString& category) {
    QMutexLocker locker(&s_logMutex);
    
    // Формируем строку лога
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString logLine = QString("[%1] [%2] [%3] %4")
                        .arg(timestamp)
                        .arg(levelStr)
                        .arg(category)
                        .arg(message);
    
    // Выводим в консоль в зависимости от уровня
    switch (level) {
        case Level::Debug:
            qDebug().noquote() << logLine;
            break;
        case Level::Info:
            qInfo().noquote() << logLine;
            break;
        case Level::Warning:
            qWarning().noquote() << logLine;
            break;
        case Level::Error:
            qCritical().noquote() << logLine;
            break;
        case Level::Critical:
            qCritical().noquote() << logLine;
            break;
    }
    
    // Записываем в файл, если включено
    if (s_fileLoggingEnabled) {
        QFile logFile(s_logFilePath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&logFile);
            stream << logLine << Qt::endl;
            logFile.close();
        }
    }
}

void Logger::setFileLogging(bool enabled, const QString& filePath) {
    QMutexLocker locker(&s_logMutex);
    s_fileLoggingEnabled = enabled;
    s_logFilePath = filePath;
    
    if (enabled) {
        qDebug() << "Логирование в файл включено:" << filePath;
    } else {
        qDebug() << "Логирование в файл отключено";
    }
}

QString Logger::levelToString(Level level) {
    switch (level) {
        case Level::Debug:    return "DEBUG";
        case Level::Info:     return "INFO ";
        case Level::Warning:  return "WARN ";
        case Level::Error:    return "ERROR";
        case Level::Critical: return "CRIT ";
        default:              return "UNKN ";
    }
}