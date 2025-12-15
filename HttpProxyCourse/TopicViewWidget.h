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
     */
    void showTopic(const Topic& topic);

signals:
    /**
     * @brief Сигнал запроса начала тестирования.
     */
    void startTestRequested();

    /**
     * @brief Сигнал запроса возврата к выбору темы.
     */
    void backRequested();

private:
    QTextBrowser* textBrowser;
    QPushButton* btnStartTest;
    QPushButton* btnBack;
};
