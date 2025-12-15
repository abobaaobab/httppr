#pragma once

#include "DomainTypes.h"
#include "Serializer.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTabWidget>
#include <QTableView>
#include <QLineEdit>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>

/**
 * @brief Виджет администрирования курса.
 * 
 * Предоставляет интерфейс для редактирования содержимого тем курса
 * и сохранения изменений.
 */
class AdminWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Конструктор виджета администрирования.
     * @param course Указатель на курс для редактирования.
     * @param parent Родительский виджет.
     */
    explicit AdminWidget(Course* course, QWidget* parent = nullptr);

    /**
     * @brief Устанавливает текущего пользователя для проверки прав доступа.
     * @param user Текущий пользователь.
     */
    void setCurrentUser(const User& user);

signals:
    /**
     * @brief Сигнал запроса возврата к главному экрану.
     */
    void backRequested();

private slots:
    /**
     * @brief Обработчик изменения выбранной темы.
     * @param index Индекс выбранной темы.
     */
    void onTopicChanged(int index);

    /**
     * @brief Обработчик сохранения изменений.
     */
    void onSaveClicked();

    /**
     * @brief Обработчик выхода из панели администратора.
     */
    void onLogoutClicked();

    /**
     * @brief Обработчик изменения фильтра поиска студентов.
     */
    void onFilterChanged();

    /**
     * @brief Обработчик обновления статистики студентов.
     */
    void onRefreshStatistics();

private:
    /**
     * @brief Настраивает пользовательский интерфейс.
     */
    void setupUi();

    /**
     * @brief Создает вкладку редактирования курса.
     */
    QWidget* createCourseEditTab();

    /**
     * @brief Создает вкладку статистики студентов.
     */
    QWidget* createStudentStatisticsTab();

    /**
     * @brief Загружает список тем в комбобокс.
     */
    void loadTopics();

    /**
     * @brief Обновляет статистику студентов.
     */
    void updateStudentStatistics();

    /**
     * @brief Настраивает права доступа к вкладкам.
     */
    void setupAccessRights();

    // Данные
    Course* m_course;
    User m_currentUser;

    // Основные элементы интерфейса
    QVBoxLayout* m_layout;
    QTabWidget* m_tabWidget;
    QPushButton* m_btnLogout;

    // Вкладка редактирования курса
    QLabel* m_lblHeader;
    QComboBox* m_cbTopics;
    QTextEdit* m_txtHtmlEditor;
    QPushButton* m_btnSave;

    // Вкладка статистики студентов
    QLineEdit* m_filterEdit;
    QTableView* m_statisticsTable;
    QSqlQueryModel* m_statisticsModel;
    QSortFilterProxyModel* m_proxyModel;
    QPushButton* m_btnRefreshStats;
};
