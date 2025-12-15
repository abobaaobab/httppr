#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include <QDataStream>
#include <QDebug>

/**
 * @brief Структура пользователя системы.
 */
struct User {
    /// @brief Уникальный идентификатор пользователя.
    int id;
    
    /// @brief Логин пользователя.
    QString login;
    
    /// @brief Полное имя пользователя.
    QString fullName;
    
    /// @brief Роль пользователя (admin, student).
    QString role;
    
    /**
     * @brief Конструктор по умолчанию.
     */
    User() : id(-1) {}
    
    /**
     * @brief Конструктор с параметрами.
     */
    User(int userId, const QString& userLogin, const QString& userFullName, const QString& userRole)
        : id(userId), login(userLogin), fullName(userFullName), role(userRole) {}
    
    /**
     * @brief Проверяет, является ли пользователь валидным.
     */
    bool isValid() const { return id > 0 && !login.isEmpty(); }
    
    /**
     * @brief Проверяет, является ли пользователь администратором.
     */
    bool isAdmin() const { return role == "admin"; }
};

/**
 * @brief Структура отдельного вопроса в тесте.
 */
struct Question {
    /// @brief Текст вопроса.
    QString text;

    /// @brief Список вариантов ответов.
    QStringList variants;

    /// @brief Индекс правильного ответа (0-based).
    /// @note Должен быть в диапазоне [0, variants.size() - 1].
    qint32 correctIndex;

    /**
     * @brief Оператор записи в QDataStream.
     * @param out Поток вывода.
     * @param q Объект вопроса.
     */
    friend QDataStream& operator<<(QDataStream& out, const Question& q) {
        out << q.text << q.variants << q.correctIndex;
        return out;
    }

    /**
     * @brief Оператор чтения из QDataStream.
     * @param in Поток ввода.
     * @param q Объект вопроса для заполнения.
     */
    friend QDataStream& operator>>(QDataStream& in, Question& q) {
        in >> q.text >> q.variants >> q.correctIndex;
        
        // Критическая валидация после десериализации
        // Проверяем, что variants не пуст и correctIndex в допустимых границах
        if (q.variants.isEmpty()) {
            qWarning() << "Question deserialization: empty variants list, adding default options";
            q.variants << "Да" << "Нет";
            q.correctIndex = 0;
        } else if (q.correctIndex < 0 || q.correctIndex >= q.variants.size()) {
            qWarning() << "Question deserialization: invalid correctIndex" << q.correctIndex 
                      << "for" << q.variants.size() << "variants, reset to 0";
            q.correctIndex = 0; // Устанавливаем безопасное значение по умолчанию
        }
        
        return in;
    }
};

/**
 * @brief Структура темы обучения.
 * Содержит теоретический материал и список вопросов для самопроверки.
 */
struct Topic {
    /// @brief Заголовок темы.
    QString title;

    /// @brief Содержание теории в формате HTML4 (для QTextBrowser).
    QString htmlContent;

    /// @brief Список вопросов по данной теме.
    QList<Question> questions;

    friend QDataStream& operator<<(QDataStream& out, const Topic& t) {
        out << t.title << t.htmlContent << t.questions;
        return out;
    }

    friend QDataStream& operator>>(QDataStream& in, Topic& t) {
        in >> t.title >> t.htmlContent >> t.questions;
        return in;
    }
};

/**
 * @brief Корневая структура курса.
 * Содержит упорядоченный список всех тем.
 */
struct Course {
    /// @brief Список тем курса.
    QList<Topic> topics;

    friend QDataStream& operator<<(QDataStream& out, const Course& c) {
        out << c.topics;
        return out;
    }

    friend QDataStream& operator>>(QDataStream& in, Course& c) {
        in >> c.topics;
        return in;
    }
};
