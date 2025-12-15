#pragma once

#include "DomainTypes.h"
#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>

/**
 * @brief Виджет просмотра содержимого темы.
 * 
 * Отображает теоретический материал темы и предоставляет возможность
 * перейти к тестированию или вернуться к выбору темы.
 */
class TopicViewWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета просмотра темы.
     * @param parent Родительский виджет.
     */
    explicit TopicViewWidget(QWidget *parent = nullptr);

    /**
     * @brief Отображает содержимое указанной темы.
     * @param topic Тема для отображения.
     * @param topicIndex Индекс темы для обновления прогресса.
     */
    void showTopic(const Topic& topic, int topicIndex);

signals:
    /**
     * @brief Сигнал запроса начала тестирования.
     */
    void startTestRequested();

    /**
     * @brief Сигнал запроса возврата к выбору темы.
     */
    void backRequested();

    /**
     * @brief Сигнал запроса обновления прогресса пользователя.
     * @param topicIndex Индекс изученной темы.
     */
    void progressUpdateRequested(int topicIndex);

protected:
    /**
     * @brief Обработчик события закрытия виджета.
     * Обновляет прогресс пользователя при закрытии теории.
     */
    void closeEvent(QCloseEvent* event) override;

private slots:
    /**
     * @brief Обработчик нажатия кнопки "Назад".
     * Обновляет прогресс перед возвратом.
     */
    void onBackClicked();

private:
    /**
     * @brief Обновляет прогресс пользователя.
     */
    void updateUserProgress();

    QTextBrowser* textBrowser;
    QPushButton* btnStartTest;
    QPushButton* btnBack;
    int currentTopicIndex;  ///< Индекс текущей темы
};
