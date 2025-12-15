# DatabaseManager Usage Example

## Overview
The `DatabaseManager` class provides a singleton interface for PostgreSQL database operations in the HTTP Proxy Course application.

## Basic Usage

### 1. Initialize Database Connection
```cpp
#include "DatabaseManager.h"

// Get the singleton instance
DatabaseManager& dbManager = DatabaseManager::instance();

// Connect to database
if (!dbManager.connectDb()) {
    qCritical() << "Failed to connect to database";
    return false;
}

// Initialize schema (creates tables and seeds data)
if (!dbManager.initSchema()) {
    qCritical() << "Failed to initialize database schema";
    return false;
}
```

### 2. Execute Custom Queries
```cpp
// Get database reference for custom queries
QSqlDatabase& db = dbManager.database();

QSqlQuery query(db);
query.prepare("SELECT * FROM users WHERE role = ?");
query.addBindValue("admin");

if (query.exec()) {
    while (query.next()) {
        QString login = query.value("login").toString();
        QString fullName = query.value("full_name").toString();
        qDebug() << "User:" << login << "(" << fullName << ")";
    }
}
```

### 3. User Authentication Example
```cpp
bool authenticateUser(const QString& login, const QString& password) {
    if (!DatabaseManager::instance().isConnected()) {
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT pass_hash FROM users WHERE login = ?");
    query.addBindValue(login);
    
    if (query.exec() && query.next()) {
        QString storedHash = query.value("pass_hash").toString();
        QString inputHash = calculateSha256(password);
        return storedHash == inputHash;
    }
    
    return false;
}
```

### 4. Progress Tracking Example
```cpp
bool updateUserProgress(int userId, int topicId) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO progress (user_id, last_topic_id, updated_at) 
        VALUES (?, ?, CURRENT_TIMESTAMP)
        ON CONFLICT (user_id) 
        DO UPDATE SET last_topic_id = ?, updated_at = CURRENT_TIMESTAMP
    )");
    
    query.addBindValue(userId);
    query.addBindValue(topicId);
    query.addBindValue(topicId);
    
    return query.exec();
}
```

### 5. Test Results Storage Example
```cpp
bool saveTestResult(int userId, int score, int maxScore) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO test_results (user_id, test_date, score, max_score) 
        VALUES (?, CURRENT_TIMESTAMP, ?, ?)
    )");
    
    query.addBindValue(userId);
    query.addBindValue(score);
    query.addBindValue(maxScore);
    
    return query.exec();
}
```

## Database Schema

### Users Table
- `id` (SERIAL PRIMARY KEY)
- `login` (VARCHAR UNIQUE)
- `pass_hash` (VARCHAR) - SHA-256 hash
- `full_name` (VARCHAR)
- `role` (VARCHAR)

### Progress Table
- `user_id` (INTEGER FK to users.id)
- `last_topic_id` (INTEGER)
- `updated_at` (TIMESTAMP)

### Test Results Table
- `id` (SERIAL PRIMARY KEY)
- `user_id` (INTEGER FK to users.id)
- `test_date` (TIMESTAMP)
- `score` (INTEGER)
- `max_score` (INTEGER)

## Default Admin User
- **Login**: admin
- **Password**: admin
- **Role**: admin
- **Full Name**: System Administrator

## Connection Parameters (Hardcoded)
- **Host**: localhost
- **Database**: proxy_course
- **Username**: postgres
- **Password**: postgres
- **Port**: 5432

## Important Notes
1. The DatabaseManager is thread-safe using QMutex
2. Database connection is established once and reused
3. Schema initialization is idempotent (safe to run multiple times)
4. Topics and Questions remain in binary files as per hybrid architecture requirement
5. Only user management, progress tracking, and test results are stored in PostgreSQL