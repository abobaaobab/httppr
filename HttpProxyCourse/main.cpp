#include "mainwindow.h"
#include "CourseDataConverter.h"
#include "DatabaseManager.h"
#include "Logger.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Инициализация системы логирования
    Logger::setFileLogging(true, "application.log");
    Logger::info("Запуск приложения HTTP Proxy Course", "Main");
    
    // Проверяем наличие бинарного файла данных курса и создаем его при необходимости
    const QString courseDataFile = "course.dat";
    const QString sourceJsonFile = "source.json";
    
    if (!QFile::exists(courseDataFile)) {
        qDebug() << "Binary course data file not found. Converting from" << sourceJsonFile;
        if (QFile::exists(sourceJsonFile)) {
            if (CourseDataConverter::convertJsonToBinary(sourceJsonFile, courseDataFile)) {
                qDebug() << "Course data converted to binary format successfully!";
            } else {
                qCritical() << "Failed to convert course data to binary format.";
                qCritical() << "Application may not work properly without course data.";
            }
        } else {
            qCritical() << "Source JSON file not found:" << sourceJsonFile;
            qCritical() << "Cannot create course data file.";
        }
    } else {
        qDebug() << "Binary course data file found:" << courseDataFile;
    }
    
    // Инициализация базы данных
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.connectDb()) {
        qCritical() << "Failed to connect to database. Application will continue with limited functionality.";
    } else {
        qDebug() << "Database connected successfully";
        if (!dbManager.initSchema()) {
            qCritical() << "Failed to initialize database schema. Application will continue with limited functionality.";
        } else {
            qDebug() << "Database schema initialized successfully";
        }
    }
    
    MainWindow w;
    w.show();
    
    Logger::info("Главное окно отображено", "Main");
    
    int result = a.exec();
    
    Logger::info("Завершение работы приложения", "Main");
    return result;
}
