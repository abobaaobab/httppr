#include "mainwindow.h"
#include "Serializer.h"
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
    
    if (!QFile::exists(courseDataFile)) {
        qDebug() << "Course data file not found. Generating binary course data...";
        try {
            Serializer::generateCourseData(courseDataFile);
            qDebug() << "Course data file created successfully!";
        } catch (const std::exception& e) {
            qCritical() << "Failed to generate course data:" << e.what();
            qCritical() << "Application may not work properly without course data.";
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
