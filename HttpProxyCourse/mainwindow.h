#pragma once

#include "SessionManager.h"
#include "AuthService.h"
#include "LoginWidget.h"
#include "TopicSelectionWidget.h"
#include "TopicViewWidget.h"
#include "TestWidget.h"
#include "AdminWidget.h"
#include "StudentProfileWidget.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>

/**
 * @brief Главное окно приложения для изучения HTTP Proxy курса.
 * 
 * Управляет навигацией между различными экранами приложения:
 * авторизация, выбор темы, просмотр материала, тестирование и администрирование.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна.
     * @param parent Родительский виджет.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор главного окна.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Обработчик запуска студенческой сессии.
     */
    void handleStudentStart();

    /**
     * @brief Обработчик успешной аутентификации пользователя.
     * @param user Аутентифицированный пользователь.
     */
    void handleUserAuthenticated(const User& user);

    /**
     * @brief Обработчик попытки входа администратора.
     * @param password Введенный пароль.
     */
    void handleAdminLogin(const QString& password);

    /**
     * @brief Обработчик выхода из системы.
     */
    void handleLogout();

    /**
     * @brief Обработчик возврата из панели администратора.
     */
    void onAdminBackRequested();

    /**
     * @brief Обработчик выбора темы студентом.
     * @param index Индекс выбранной темы.
     */
    void onTopicSelected(int index);

    /**
     * @brief Обработчик запуска тестирования.
     */
    void onStartTestRequested();

    /**
     * @brief Обработчик отправки ответа на вопрос.
     * @param answerIndex Индекс выбранного ответа.
     */
    void onAnswerSubmitted(int answerIndex);

    /**
     * @brief Обработчик завершения теста.
     * @param score Набранные баллы.
     * @param maxScore Максимальные баллы.
     * @param timeExpired true, если время истекло.
     */
    void onTestFinished(int score, int maxScore, bool timeExpired);

    /**
     * @brief Показать диалог "О программе".
     */
    void showAboutDialog();

    /**
     * @brief Обработчик запроса показа профиля студента.
     */
    void onShowProfileRequested();

private:
    /**
     * @brief Загружает данные курса из файла.
     */
    void loadCourseData();

    /**
     * @brief Инициализирует меню приложения.
     */
    void setupMenu();

    SessionManager m_sessionManager;
    QStackedWidget *m_stackedWidget;
    LoginWidget *m_loginWidget;
    TopicSelectionWidget *m_topicWidget;
    TopicViewWidget *m_topicViewWidget;
    TestWidget *m_testWidget;
    AdminWidget *m_adminWidget;
    StudentProfileWidget *m_studentProfileWidget;
};
