#include "UserDao.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

User UserDao::findByLogin(const QString& login) {
    User user;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in UserDao::findByLogin";
        return user;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT id, login, full_name, role FROM users WHERE login = ?");
    query.addBindValue(login.trimmed());
    
    if (!query.exec()) {
        qCritical() << "Failed to execute findByLogin query:" << query.lastError().text();
        return user;
    }
    
    if (query.next()) {
        user.id = query.value("id").toInt();
        user.login = query.value("login").toString();
        user.fullName = query.value("full_name").toString();
        user.role = query.value("role").toString();
    }
    
    return user;
}

bool UserDao::create(const User& user, const QString& passwordHash) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in UserDao::create";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("INSERT INTO users (login, password_hash, full_name, role) VALUES (?, ?, ?, ?)");
    query.addBindValue(user.login.trimmed());
    query.addBindValue(passwordHash);
    query.addBindValue(user.fullName.trimmed());
    query.addBindValue(user.role);
    
    if (!query.exec()) {
        qCritical() << "Failed to create user:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool UserDao::existsByLogin(const QString& login) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in UserDao::existsByLogin";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT COUNT(*) FROM users WHERE login = ?");
    query.addBindValue(login.trimmed());
    
    if (!query.exec()) {
        qCritical() << "Failed to check user existence:" << query.lastError().text();
        return false;
    }
    
    return query.next() && query.value(0).toInt() > 0;
}

QList<User> UserDao::findAll() {
    QList<User> users;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in UserDao::findAll";
        return users;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT id, login, full_name, role FROM users ORDER BY id");
    
    if (!query.exec()) {
        qCritical() << "Failed to fetch all users:" << query.lastError().text();
        return users;
    }
    
    while (query.next()) {
        User user;
        user.id = query.value("id").toInt();
        user.login = query.value("login").toString();
        user.fullName = query.value("full_name").toString();
        user.role = query.value("role").toString();
        users.append(user);
    }
    
    return users;
}

bool UserDao::update(const User& user) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in UserDao::update";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("UPDATE users SET login = ?, full_name = ?, role = ? WHERE id = ?");
    query.addBindValue(user.login.trimmed());
    query.addBindValue(user.fullName.trimmed());
    query.addBindValue(user.role);
    query.addBindValue(user.id);
    
    if (!query.exec()) {
        qCritical() << "Failed to update user:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

bool UserDao::deleteById(int userId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in UserDao::deleteById";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to delete user:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}