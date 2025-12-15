#include "AuthService.h"
#include "UserDao.h"
#include "DatabaseManager.h"
#include "Logger.h"
#include <QCryptographicHash>
#include <QByteArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDebug>

AuthService::AuthResult AuthService::login(const QString& login, const QString& password, User& user) {
    // Валидация входных данных
    if (login.trimmed().isEmpty() || password.isEmpty()) {
        return AuthResult::InvalidCredentials;
    }

    // Проверка подключения к БД
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qCritical() << "Database not connected for authentication";
        return AuthResult::DatabaseError;
    }

    // Поиск пользователя через DAO
    User foundUser = UserDao::findByLogin(login);
    if (!foundUser.isValid()) {
        qDebug() << "User not found:" << login;
        return AuthResult::UserNotFound;
    }

    // Получение хэша пароля из БД для проверки
    QSqlQuery query(dbManager.database());
    query.prepare("SELECT password_hash FROM users WHERE login = ?");
    query.addBindValue(login.trimmed());

    if (!query.exec()) {
        qCritical() << "Failed to execute password query:" << query.lastError().text();
        return AuthResult::DatabaseError;
    }

    if (!query.next()) {
        return AuthResult::UserNotFound;
    }

    QString storedHash = query.value("password_hash").toString();

    // Проверка пароля
    QString inputHash = calculateSha256(password);
    if (inputHash != storedHash) {
        qDebug() << "Invalid password for user:" << login;
        return AuthResult::InvalidCredentials;
    }

    // Возвращаем найденного пользователя
    user = foundUser;
    qDebug() << "User authenticated successfully:" << login << "(" << foundUser.role << ")";
    
    return AuthResult::Success;
}

AuthService::RegisterResult AuthService::registerUser(const QString& login, const QString& password, 
                                                    const QString& fullName, const QString& role) {
    // Расширенная валидация входных данных
    if (login.trimmed().isEmpty() || password.isEmpty() || fullName.trimmed().isEmpty()) {
        Logger::warning("Попытка регистрации с пустыми полями", "Auth");
        return RegisterResult::InvalidInput;
    }

    // Проверка валидности логина
    if (!isLoginValid(login)) {
        Logger::warning("Попытка регистрации с невалидным логином: " + login, "Auth");
        return RegisterResult::InvalidInput;
    }

    // Проверка силы пароля
    if (!isPasswordStrong(password)) {
        Logger::warning("Попытка регистрации со слабым паролем для пользователя: " + login, "Auth");
        return RegisterResult::InvalidInput;
    }

    // Проверка подключения к БД
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.isConnected()) {
        qCritical() << "Database not connected for registration";
        return RegisterResult::DatabaseError;
    }

    // Проверка существования пользователя через DAO
    if (UserDao::existsByLogin(login)) {
        qDebug() << "User already exists:" << login;
        return RegisterResult::UserExists;
    }

    // Создание нового пользователя через DAO
    User newUser(-1, login.trimmed(), fullName.trimmed(), role);
    QString passwordHash = calculateSha256(password);
    
    if (!UserDao::create(newUser, passwordHash)) {
        qCritical() << "Failed to register user through DAO";
        return RegisterResult::DatabaseError;
    }

    qDebug() << "User registered successfully:" << login << "(" << role << ")";
    return RegisterResult::Success;
}

bool AuthService::checkAdminPassword(const QString& password) {
    User user;
    AuthResult result = login("admin", password, user);
    return result == AuthResult::Success && user.isAdmin();
}

QString AuthService::calculateSha256(const QString& input) {
    QByteArray hashBytes = QCryptographicHash::hash(
        input.toUtf8(),
        QCryptographicHash::Sha256
    );
    return QString::fromLatin1(hashBytes.toHex());
}

bool AuthService::isPasswordStrong(const QString& password) {
    // Минимальная длина пароля
    if (password.length() < 4) {
        return false;
    }

    // Для учебного проекта используем простые правила
    // В реальном проекте можно добавить проверки на:
    // - наличие цифр, букв верхнего/нижнего регистра
    // - специальные символы
    // - отсутствие в словаре популярных паролей
    
    return true; // Пока принимаем любой пароль длиннее 4 символов
}

bool AuthService::isLoginValid(const QString& login) {
    // Проверка длины логина
    if (login.length() < 3 || login.length() > 20) {
        return false;
    }

    // Проверка на допустимые символы: только латинские буквы, цифры и подчеркивание
    QRegularExpression loginRegex("^[a-zA-Z0-9_]+$");
    return loginRegex.match(login).hasMatch();
}
