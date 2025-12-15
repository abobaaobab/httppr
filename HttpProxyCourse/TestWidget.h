#pragma once

#include "DomainTypes.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QList>
#include <QTimer>
#include <QProgressBar>
#include <QDateTime>

/**
 * @brief Виджет тестирования знаний.
 * 
 * Отображает вопросы с вариантами ответов и обрабатывает выбор студента.
 */
class TestWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета тестирования.
     * @param parent Родительский виджет.
     */
    explicit TestWidget(QWidget *parent = nullptr);

    /**
     * @brief Отображает вопрос с вариантами ответов.
     * @param question Вопрос для отображения.
     */
    void showQuestion(const Question& question);

    /**
     * @brief Возвращает индекс выбранного варианта ответа.
     * @return Индекс выбранного варианта или -1, если ничего не выбрано.
     */
    int getSelectedVariantIndex();

    /**
     * @brief Начинает тест с указанными вопросами и пользователем.
     * @param questions Список вопросов для тестирования.
     * @param user Пользователь, проходящий тест.
     * @param timeLimit Лимит времени в минутах (по умолчанию 20).
     */
    void startTest(const QList<Question>& questions, const User& user, int timeLimit = 20);

    /**
     * @brief Завершает тест и сохраняет результаты.
     */
    void finishTest();

    /**
     * @brief Возвращает текущий счет.
     * @return Количество правильных ответов.
     */
    int getCurrentScore() const;

    /**
     * @brief Возвращает максимальный возможный счет.
     * @return Общее количество вопросов.
     */
    int getMaxScore() const;

signals:
    /**
     * @brief Сигнал отправки ответа.
     * @param index Индекс выбранного ответа.
     */
    void answerSubmitted(int index);

    /**
     * @brief Сигнал завершения теста.
     * @param score Набранные баллы.
     * @param maxScore Максимальные баллы.
     * @param timeExpired true, если время истекло.
     */
    void testFinished(int score, int maxScore, bool timeExpired);

private slots:
    /**
     * @brief Обработчик истечения времени.
     */
    void onTimeExpired();

    /**
     * @brief Обработчик обновления таймера.
     */
    void updateTimer();

    /**
     * @brief Обработчик отправки ответа.
     */
    void onAnswerSubmitted();

private:
    /**
     * @brief Сохраняет результат теста в базу данных.
     * @return true, если результат сохранен успешно.
     */
    bool saveTestResult();

    /**
     * @brief Обновляет отображение прогресса.
     */
    void updateProgress();

    /**
     * @brief Показывает следующий вопрос или завершает тест.
     */
    void showNextQuestion();

    // UI элементы
    QLabel* questionLabel;
    QWidget* variantsContainer;
    QVBoxLayout* variantsLayout;
    QPushButton* btnAnswer;
    QList<QRadioButton*> radioButtons;

    // Таймер и прогресс
    QTimer* m_timer;
    QTimer* m_updateTimer;
    QLabel* m_timeLabel;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;

    // Данные теста
    QList<Question> m_questions;
    User m_currentUser;
    int m_currentQuestionIndex;
    int m_correctAnswers;
    int m_timeLimitMinutes;
    QDateTime m_testStartTime;
    bool m_testActive;
};
