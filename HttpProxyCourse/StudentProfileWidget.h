#pragma once

#include "DomainTypes.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QGroupBox>
#include <QGridLayout>
#include <QSqlQuery>

/**
 * @brief Виджет профиля студента с историей тестирования.
 * 
 * Отображает информацию о пользователе и историю его тестов
 * с использованием QTableView и QSqlQueryModel.
 */
class StudentProfileWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета профиля студента.
     * @param parent Родительский виджет.
     */
    explicit StudentProfileWidget(QWidget *parent = nullptr);

    /**
     * @brief Устанавливает текущего пользователя и обновляет данные.
     * @param user Пользователь для отображения.
     */
    void setCurrentUser(const User& user);

    /**
     * @brief Устанавливает пользователя для отображения профиля.
     * @param user Пользователь для отображения.
     */
    void setUser(const User& user);

    /**
     * @brief Обновляет данные профиля и историю тестов.
     */
    void refreshData();

    /**
     * @brief Устанавливает модель результатов тестов.
     * @param model Модель результатов для отображения.
     */
    void setResultsModel(QAbstractItemModel* model);

signals:
    /**
     * @brief Сигнал запроса возврата к предыдущему экрану.
     */
    void backRequested();

private slots:
    /**
     * @brief Обработчик нажатия кнопки "Назад".
     */
    void onBackClicked();

    /**
     * @brief Обработчик нажатия кнопки "Обновить".
     */
    void onRefreshClicked();

private:
    /**
     * @brief Создает интерфейс информации о пользователе.
     */
    QWidget* createUserInfoSection();

    /**
     * @brief Создает интерфейс статистики.
     */
    QWidget* createStatisticsSection();

    /**
     * @brief Создает интерфейс истории тестов.
     */
    QWidget* createTestHistorySection();

    /**
     * @brief Обновляет информацию о пользователе.
     */
    void updateUserInfo();

    /**
     * @brief Обновляет статистику пользователя.
     */
    void updateStatistics();

    /**
     * @brief Обновляет историю тестов.
     */
    void updateTestHistory();

    // Текущий пользователь
    User m_currentUser;

    // UI элементы - информация о пользователе
    QLabel* m_userNameLabel;
    QLabel* m_userLoginLabel;
    QLabel* m_userRoleLabel;

    // UI элементы - статистика
    QLabel* m_totalTestsLabel;
    QLabel* m_averageScoreLabel;
    QLabel* m_bestScoreLabel;
    QLabel* m_lastTestLabel;

    // UI элементы - история тестов
    QTableView* m_testHistoryTable;
    QSqlQueryModel* m_testHistoryModel;

    // Кнопки управления
    QPushButton* m_backButton;
    QPushButton* m_refreshButton;
};