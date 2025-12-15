#include "TestResultDao.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool TestResultDao::save(const TestResult& result) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in TestResultDao::save";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("INSERT INTO test_results (user_id, test_date, score, max_score) VALUES (?, ?, ?, ?)");
    query.addBindValue(result.userId);
    query.addBindValue(result.testDate);
    query.addBindValue(result.score);
    query.addBindValue(result.maxScore);
    
    if (!query.exec()) {
        qCritical() << "Failed to save test result:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Test result saved for user" << result.userId 
             << "- score:" << result.score << "/" << result.maxScore;
    return true;
}

QList<TestResult> TestResultDao::findByUserId(int userId) {
    QList<TestResult> results;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in TestResultDao::findByUserId";
        return results;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT id, user_id, test_date, score, max_score FROM test_results WHERE user_id = ? ORDER BY test_date DESC");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to fetch test results:" << query.lastError().text();
        return results;
    }
    
    while (query.next()) {
        TestResult result;
        result.id = query.value("id").toInt();
        result.userId = query.value("user_id").toInt();
        result.testDate = query.value("test_date").toDateTime();
        result.score = query.value("score").toInt();
        result.maxScore = query.value("max_score").toInt();
        results.append(result);
    }
    
    return results;
}

QList<TestResult> TestResultDao::findAll() {
    QList<TestResult> results;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in TestResultDao::findAll";
        return results;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT id, user_id, test_date, score, max_score FROM test_results ORDER BY test_date DESC");
    
    if (!query.exec()) {
        qCritical() << "Failed to fetch all test results:" << query.lastError().text();
        return results;
    }
    
    while (query.next()) {
        TestResult result;
        result.id = query.value("id").toInt();
        result.userId = query.value("user_id").toInt();
        result.testDate = query.value("test_date").toDateTime();
        result.score = query.value("score").toInt();
        result.maxScore = query.value("max_score").toInt();
        results.append(result);
    }
    
    return results;
}

TestResult TestResultDao::getBestResult(int userId) {
    TestResult bestResult;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in TestResultDao::getBestResult";
        return bestResult;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare(R"(
        SELECT id, user_id, test_date, score, max_score 
        FROM test_results 
        WHERE user_id = ? 
        ORDER BY (CAST(score AS FLOAT) / max_score) DESC, test_date DESC 
        LIMIT 1
    )");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to fetch best result:" << query.lastError().text();
        return bestResult;
    }
    
    if (query.next()) {
        bestResult.id = query.value("id").toInt();
        bestResult.userId = query.value("user_id").toInt();
        bestResult.testDate = query.value("test_date").toDateTime();
        bestResult.score = query.value("score").toInt();
        bestResult.maxScore = query.value("max_score").toInt();
    }
    
    return bestResult;
}

double TestResultDao::getAverageScore(int userId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in TestResultDao::getAverageScore";
        return 0.0;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare(R"(
        SELECT AVG(CAST(score AS FLOAT) / max_score * 100) as avg_percentage 
        FROM test_results 
        WHERE user_id = ? AND max_score > 0
    )");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to calculate average score:" << query.lastError().text();
        return 0.0;
    }
    
    if (query.next()) {
        return query.value("avg_percentage").toDouble();
    }
    
    return 0.0;
}

bool TestResultDao::deleteByUserId(int userId) {
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qWarning() << "Database not connected in TestResultDao::deleteByUserId";
        return false;
    }
    
    QSqlQuery query(dbManager.database());
    query.prepare("DELETE FROM test_results WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qCritical() << "Failed to delete test results:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}