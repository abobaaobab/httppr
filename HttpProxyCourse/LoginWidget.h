#pragma once

#include "DomainTypes.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QMessageBox>
#include <QRegularExpressionValidator>

/**
 * @brief Виджет авторизации и регистрации пользователей.
 * 
 * Предоставляет интерфейс для входа в систему и регистрации новых пользователей.
 */
class LoginWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета авторизации.
     * @param parent Родительский виджет.
     */
    explicit LoginWidget(QWidget *parent = nullptr);

signals:
    /**
     * @brief Сигнал успешной аутентификации пользователя.
     * @param user Аутентифицированный пользователь.
     */
    void userAuthenticated(const User& user);

    /**
     * @brief Сигнал попытки входа администратора (для обратной совместимости).
     * @param password Введенный пароль.
     */
    void adminLoginAttempt(QString password);

    /**
     * @brief Сигнал запуска студенческой сессии (для обратной совместимости).
     */
    void startStudentSession();

private slots:
    /**
     * @brief Обработчик нажатия кнопки входа.
     */
    void onLoginClicked();

    /**
     * @brief Обработчик нажатия кнопки регистрации.
     */
    void onRegisterClicked();

    /**
     * @brief Обработчик нажатия кнопки входа администратора (для обратной совместимости).
     */
    void onAdminLoginClicked();

private:
    /**
     * @brief Создает интерфейс вкладки входа.
     */
    QWidget* createLoginTab();

    /**
     * @brief Создает интерфейс вкладки регистрации.
     */
    QWidget* createRegisterTab();

    /**
     * @brief Создает интерфейс быстрого доступа.
     */
    QWidget* createQuickAccessTab();

    /**
     * @brief Валидирует поля ввода.
     * @param login Логин.
     * @param password Пароль.
     * @param fullName Полное имя (опционально).
     * @return true, если данные валидны.
     */
    bool validateInput(const QString& login, const QString& password, const QString& fullName = QString());

    /**
     * @brief Показывает сообщение об ошибке.
     * @param message Текст сообщения.
     */
    void showError(const QString& message);

    /**
     * @brief Показывает сообщение об успехе.
     * @param message Текст сообщения.
     */
    void showSuccess(const QString& message);

    // Вкладки
    QTabWidget* m_tabWidget;

    // Вкладка входа
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginButton;

    // Вкладка регистрации
    QLineEdit* m_regLoginEdit;
    QLineEdit* m_regPasswordEdit;
    QLineEdit* m_regFullNameEdit;
    QPushButton* m_registerButton;

    // Вкладка быстрого доступа (для обратной совместимости)
    QLineEdit* m_adminPasswordEdit;
    QPushButton* m_adminLoginButton;
    QPushButton* m_studentStartButton;
};
