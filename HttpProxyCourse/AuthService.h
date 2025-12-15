#pragma once

#include "DomainTypes.h"
#include <QString>
#include <QSqlError>

/**
 * @brief Сервис аутентификации пользователей.
 *
 * Обеспечивает аутентификацию и регистрацию пользователей через базу данных.
 */
class AuthService {
public:
    /**
     * @brief Результат операции аутентификации.
     */
    enum class AuthResult {
        Success,            ///< Успешная аутентификация
        InvalidCredentials, ///< Неверные учетные данные
        UserNotFound,       ///< Пользователь не найден
        DatabaseError       ///< Ошибка базы данных
    };
    
    /**
     * @brief Результат операции регистрации.
     */
    enum class RegisterResult {
        Success,            ///< Успешная регистрация
        UserExists,         ///< Пользователь уже существует
        InvalidInput,       ///< Некорректные входные данные
        DatabaseError       ///< Ошибка базы данных
    };

    /**
     * @brief Аутентификация пользователя.
     * @param login Логин пользователя.
     * @param password Пароль в открытом виде.
     * @param user Ссылка на объект User для заполнения данными.
     * @return Результат аутентификации.
     */
    static AuthResult login(const QString& login, const QString& password, User& user);

    /**
     * @brief Регистрация нового пользователя.
     * @param login Логин пользователя.
     * @param password Пароль в открытом виде.
     * @param fullName Полное имя пользователя.
     * @param role Роль пользователя (по умолчанию "student").
     * @return Результат регистрации.
     */
    static RegisterResult registerUser(const QString& login, const QString& password, 
                                     const QString& fullName, const QString& role = "student");

    /**
     * @brief Проверяет валидность пароля администратора (для обратной совместимости).
     * @param password Введенный пароль в открытом виде.
     * @return true, если пользователь является администратором.
     */
    static bool checkAdminPassword(const QString& password);

    /**
     * @brief Проверяет силу пароля.
     * @param password Пароль для проверки.
     * @return true, если пароль достаточно сильный.
     */
    static bool isPasswordStrong(const QString& password);

    /**
     * @brief Проверяет валидность логина.
     * @param login Логин для проверки.
     * @return true, если логин валиден.
     */
    static bool isLoginValid(const QString& login);

private:
    /**
     * @brief Вычисляет SHA-256 хэш строки.
     * @param input Входная строка.
     * @return SHA-256 хэш в виде hex-строки.
     */
    static QString calculateSha256(const QString& input);
};
