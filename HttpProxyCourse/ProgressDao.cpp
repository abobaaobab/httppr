#include "ProgressDao.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

UserProgress ProgressDao::findByUserId(int userId) {
    UserProgress progress;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in ProgressDao::findByUserId";
        return progress;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT user_id, last_topic_id, updated_at FROM progress WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to execute findByUserId query:" << query.lastError().text();
        return progress;
    }
    
    if (query.next()) {
        progress.userId = query.value("user_id").toInt();
        progress.lastTopicId = query.value("last_topic_id").toInt();
        progress.updatedAt = query.value("updated_at").toDateTime();
    }
    
    return progress;
}

bool ProgressDao::updateProgress(int userId, int topicId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in ProgressDao::updateProgress";
        return false;
    }
    
    // Сначала проверяем, существует ли запись
    UserProgress existing = findByUserId(userId);
    
    QSqlQuery query(dbManager.database());
    
    if (existing.userId > 0) {
        // Обновляем существующую запись
        query.prepare("UPDATE progress SET last_topic_id = ?, updated_at = CURRENT_TIMESTAMP WHERE user_id = ?");
        query.addBindValue(topicId);
        query.addBindValue(userId);
    } else {
        // Создаем новую запись
        query.prepare("INSERT INTO progress (user_id, last_topic_id, updated_at) VALUES (?, ?, CURRENT_TIMESTAMP)");
        query.addBindValue(userId);
        query.addBindValue(topicId);
    }
    
    if (!query.exec()) {
        qCritical() << "Failed to update progress:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Progress updated for user" << userId << "to topic" << topicId;
    return true;
}

bool ProgressDao::createProgress(int userId, int topicId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in ProgressDao::createProgress";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("INSERT INTO progress (user_id, last_topic_id, updated_at) VALUES (?, ?, CURRENT_TIMESTAMP)");
    query.addBindValue(userId);
    query.addBindValue(topicId);
    
    if (!query.exec()) {
        qCritical() << "Failed to create progress:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ProgressDao::deleteByUserId(int userId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in ProgressDao::deleteByUserId";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("DELETE FROM progress WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to delete progress:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}