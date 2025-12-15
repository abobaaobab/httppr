#pragma once

#include "DomainTypes.h"
#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

/**
 * @brief Виджет выбора темы для изучения.
 * 
 * Отображает список доступных тем курса и позволяет студенту выбрать тему для изучения.
 */
class TopicSelectionWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета выбора темы.
     * @param parent Родительский виджет.
     */
    explicit TopicSelectionWidget(QWidget *parent = nullptr);

    /**
     * @brief Устанавливает список тем для отображения.
     * @param topics Список тем курса.
     */
    void setTopics(const QList<Topic>& topics);

    /**
     * @brief Устанавливает последнюю изученную тему.
     * @param topicId Идентификатор последней изученной темы.
     */
    void setLastStudiedTopic(int topicId);

signals:
    /**
     * @brief Сигнал выбора темы.
     * @param index Индекс выбранной темы.
     */
    void topicSelected(int index);

    /**
     * @brief Сигнал запроса выхода из системы.
     */
    void logoutRequested();

    /**
     * @brief Сигнал запроса показа профиля студента.
     */
    void profileRequested();

private slots:
    /**
     * @brief Обработчик нажатия кнопки выбора темы.
     */
    void onSelectClicked();

    /**
     * @brief Обработчик двойного клика по элементу списка.
     * @param item Элемент списка.
     */
    void onListDoubleClicked(QListWidgetItem *item);

    /**
     * @brief Обработчик нажатия кнопки профиля.
     */
    void onProfileClicked();

private:
    QLabel *m_titleLabel;
    QListWidget *m_topicsList;
    QPushButton *m_selectButton;
    QPushButton *m_profileButton;
    QPushButton *m_logoutButton;
};
