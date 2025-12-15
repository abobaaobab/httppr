#include "TopicViewWidget.h"
#include "ProgressDao.h"
#include "SessionManager.h"
#include <QCloseEvent>
#include <QDebug>

TopicViewWidget::TopicViewWidget(QWidget *parent) : QWidget(parent), currentTopicIndex(-1) {
    auto layout = new QVBoxLayout(this);

    textBrowser = new QTextBrowser(this);
    textBrowser->setReadOnly(true);
    textBrowser->setOpenExternalLinks(false);

    btnStartTest = new QPushButton("Перейти к тесту", this);
    btnBack = new QPushButton("Назад к списку", this);

    layout->addWidget(textBrowser);
    layout->addWidget(btnStartTest);
    layout->addWidget(btnBack);

    connect(btnStartTest, &QPushButton::clicked, this, &TopicViewWidget::startTestRequested);
    connect(btnBack, &QPushButton::clicked, this, &TopicViewWidget::onBackClicked);
}

void TopicViewWidget::showTopic(const Topic& topic, int topicIndex) {
    textBrowser->setHtml(topic.htmlContent);
    currentTopicIndex = topicIndex;
    qDebug() << "Showing topic" << topicIndex << ":" << topic.title;
}

void TopicViewWidget::closeEvent(QCloseEvent* event) {
    updateUserProgress();
    QWidget::closeEvent(event);
}

void TopicViewWidget::onBackClicked() {
    updateUserProgress();
    emit backRequested();
}

void TopicViewWidget::updateUserProgress() {
    if (currentTopicIndex < 0) {
        return; // Нет активной темы
    }
    
    // Отправляем сигнал для обновления прогресса
    emit progressUpdateRequested(currentTopicIndex);
}
