#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QMutexLocker>

DatabaseManager::DatabaseManager() : m_connected(false) {
    // Инициализация подключения к БД
    m_database = QSqlDatabase::addDatabase("QPSQL");
}

DatabaseManager::~DatabaseManager() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connectDb() {
    QMutexLocker locker(&m_mutex);
    
    if (m_connected && m_database.isOpen()) {
        return true;
    }

    // Загружаем конфигурацию БД из файла
    DatabaseConfig& config = DatabaseConfig::instance();
    config.loadConfig();

    // Настройка параметров подключения из конфигурации
    m_database.setHostName(config.hostName());
    m_database.setDatabaseName(config.databaseName());
    m_database.setUserName(config.userName());
    m_database.setPassword(config.password());
    m_database.setPort(config.port());

    if (!m_database.open()) {
        qCritical() << "Failed to connect to database:" << m_database.lastError().text();
        m_connected = false;
        return false;
    }

    qDebug() << "Successfully connected to PostgreSQL database";
    m_connected = true;
    return true;
}

bool DatabaseManager::initSchema() {
    if (!isConnected()) {
        qWarning() << "Cannot initialize schema: database not connected";
        return false;
    }

    if (!createTables()) {
        qCritical() << "Failed to create database tables";
        return false;
    }

    if (!seedData()) {
        qCritical() << "Failed to seed initial data";
        return false;
    }

    qDebug() << "Database schema initialized successfully";
    return true;
}

bool DatabaseManager::isConnected() const {
    return m_connected && m_database.isOpen();
}

QSqlDatabase& DatabaseManager::database() {
    return m_database;
}

bool DatabaseManager::createTables() {
    QSqlQuery query(m_database);

    // Создание таблицы users согласно ТЗ
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            login VARCHAR(255) UNIQUE NOT NULL,
            password_hash VARCHAR(255) NOT NULL,
            full_name VARCHAR(255),
            role VARCHAR(50) NOT NULL CHECK (role IN ('student', 'admin'))
        )
    )";

    if (!query.exec(createUsersTable)) {
        qCritical() << "Failed to create users table:" << query.lastError().text();
        return false;
    }

    // Создание таблицы progress
    QString createProgressTable = R"(
        CREATE TABLE IF NOT EXISTS progress (
            user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
            last_topic_id INTEGER NOT NULL,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (user_id)
        )
    )";

    if (!query.exec(createProgressTable)) {
        qCritical() << "Failed to create progress table:" << query.lastError().text();
        return false;
    }

    // Создание таблицы test_results
    QString createTestResultsTable = R"(
        CREATE TABLE IF NOT EXISTS test_results (
            id SERIAL PRIMARY KEY,
            user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
            test_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            score INTEGER NOT NULL,
            max_score INTEGER NOT NULL
        )
    )";

    if (!query.exec(createTestResultsTable)) {
        qCritical() << "Failed to create test_results table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database tables created successfully";
    return true;
}

bool DatabaseManager::seedData() {
    QSqlQuery query(m_database);

    // Проверяем, пуста ли таблица users
    if (!query.exec("SELECT COUNT(*) FROM users")) {
        qCritical() << "Failed to check users table:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        qDebug() << "Users table already contains data, skipping seeding";
        return true;
    }

    // Вставляем администратора по умолчанию
    QString adminPasswordHash = calculateSha256("admin");
    
    query.prepare(R"(
        INSERT INTO users (login, password_hash, full_name, role) 
        VALUES (?, ?, ?, ?)
    )");
    
    query.addBindValue("admin");
    query.addBindValue(adminPasswordHash);
    query.addBindValue("System Administrator");
    query.addBindValue("admin");

    if (!query.exec()) {
        qCritical() << "Failed to insert default admin user:" << query.lastError().text();
        return false;
    }

    qDebug() << "Default admin user created successfully";
    qDebug() << "Login: admin, Password: admin";
    return true;
}

QString DatabaseManager::calculateSha256(const QString& input) const {
    QByteArray hashBytes = QCryptographicHash::hash(
        input.toUtf8(),
        QCryptographicHash::Sha256
    );
    return QString::fromLatin1(hashBytes.toHex());
}