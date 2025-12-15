#pragma once

#include <QObject>
#include <QTimer>
#include <QStackedWidget>
#include "DomainTypes.h"
#include "CourseModel.h"
#include "SessionManager.h"

// Forward declarations
class LoginWidget;
class TopicSelectionWidget;
class TopicViewWidget;
class TestWidget;
class AdminWidget;
class StudentProfileWidget;

/**
 * @brief Главный контроллер приложения (Controller в архитектуре MVC).
 * 
 * Управляет навигацией между экранами, обработкой пользовательских действий
 * и координацией работы моделей и представлений.
 */
class AppController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Конструктор контроллера приложения.
     * @param stackedWidget Виджет для управления экранами.
     * @param parent Родительский объект для управления памятью.
     */
    explicit AppController(QStackedWidget* stackedWidget, QObject* parent = nullptr);

    /**
     * @brief Инициализация контроллера и загрузка данных.
     * @return true, если инициализация прошла успешно.
     */
    bool initialize();

    /**
     * @brief Получить модель курса.
     * @return Указатель на модель курса.
     */
    CourseModel* getCourseModel() const { return m_courseModel; }

    /**
     * @brief Получить модель результатов тестов.
     * @return Указатель на модель результатов.
     */
    TestResultsModel* getTestResultsModel() const { return m_testResultsModel; }

public slots:
    /**
     * @brief Обработчик успешной аутентификации пользователя.
     * @param user Аутентифицированный пользователь.
     */
    void handleUserAuthenticated(const User& user);

    /**
     * @brief Обработчик выхода из системы.
     */
    void handleLogout();

    /**
     * @brief Обработчик выбора темы студентом.
     * @param topicIndex Индекс выбранной темы.
     */
    void handleTopicSelected(int topicIndex);

    /**
     * @brief Обработчик запуска тестирования.
     */
    void handleStartTest();

    /**
     * @brief Обработчик отправки ответа на вопрос.
     * @param answerIndex Индекс выбранного ответа.
     */
    void handleAnswerSubmitted(int answerIndex);

    /**
     * @brief Обработчик запроса показа профиля студента.
     */
    void handleShowProfile();

    /**
     * @brief Обработчик запроса обновления прогресса пользователя.
     * @param topicIndex Индекс изученной темы.
     */
    void handleProgressUpdate(int topicIndex);

    /**
     * @brief Обработчик возврата из панели администратора.
     */
    void handleAdminBack();

    /**
     * @brief Обработчик истечения времени тестирования.
     */
    void handleTestTimeout();



signals:
    /**
     * @brief Сигнал об ошибке в работе контроллера.
     * @param errorMessage Сообщение об ошибке.
     */
    void errorOccurred(const QString& errorMessage);

    /**
     * @brief Сигнал о необходимости показать сообщение пользователю.
     * @param title Заголовок сообщения.
     * @param message Текст сообщения.
     */
    void showMessage(const QString& title, const QString& message);

private slots:
    /**
     * @brief Обработчик ошибок модели курса.
     * @param errorMessage Сообщение об ошибке.
     */
    void handleCourseModelError(const QString& errorMessage);

    /**
     * @brief Обработчик ошибок модели результатов.
     * @param errorMessage Сообщение об ошибке.
     */
    void handleTestResultsError(const QString& errorMessage);

private:
    /**
     * @brief Создает и настраивает все виджеты приложения.
     */
    void setupViews();

    /**
     * @brief Подключает сигналы и слоты между компонентами.
     */
    void connectSignals();

    /**
     * @brief Переключается на указанный экран.
     * @param widget Виджет для отображения.
     */
    void switchToView(QWidget* widget);

    /**
     * @brief Начинает новый тест для текущего пользователя.
     */
    void startNewTest();

    /**
     * @brief Завершает текущий тест и сохраняет результат.
     */
    void finishTest();

    /**
     * @brief Показывает следующий вопрос в тесте.
     */
    void showNextQuestion();

    /**
     * @brief Сохраняет прогресс студента в базу данных.
     * @param userId ID пользователя.
     * @param topicId ID последней изученной темы.
     * @return true, если сохранение прошло успешно.
     */
    bool saveStudentProgress(int userId, int topicId);

    /**
     * @brief Загружает прогресс студента из базы данных.
     * @param userId ID пользователя.
     * @return ID последней изученной темы или -1 при ошибке.
     */
    int loadStudentProgress(int userId);

    // Модели данных
    CourseModel* m_courseModel;              ///< Модель данных курса
    TestResultsModel* m_testResultsModel;    ///< Модель результатов тестов

    // Управление сессией
    SessionManager m_sessionManager;         ///< Менеджер сессий пользователей

    // Виджеты (Views)
    QStackedWidget* m_stackedWidget;         ///< Контейнер для переключения экранов
    LoginWidget* m_loginWidget;              ///< Экран авторизации
    TopicSelectionWidget* m_topicWidget;     ///< Экран выбора темы
    TopicViewWidget* m_topicViewWidget;      ///< Экран просмотра теории
    TestWidget* m_testWidget;                ///< Экран тестирования
    AdminWidget* m_adminWidget;              ///< Панель администратора
    StudentProfileWidget* m_profileWidget;   ///< Профиль студента

    // Состояние тестирования
    QTimer* m_testTimer;                     ///< Таймер для ограничения времени теста
    int m_currentTopicIndex;                 ///< Индекс текущей темы
    int m_currentQuestionIndex;              ///< Индекс текущего вопроса
    int m_correctAnswers;                    ///< Количество правильных ответов
    QList<int> m_userAnswers;                ///< Ответы пользователя

    // Константы
    static const int TEST_TIME_LIMIT_MS = 300000;  ///< Лимит времени на тест (5 минут)
};