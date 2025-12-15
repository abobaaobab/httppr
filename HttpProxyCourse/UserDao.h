#pragma once

#include "DomainTypes.h"
#include <QString>
#include <QList>

/**
 * @brief Data Access Object для работы с пользователями в БД.
 * 
 * Инкапсулирует все операции с таблицей users, обеспечивая четкое
 * разделение между бизнес-логикой и доступом к данным.
 */
class UserDao {
public:
    /**
     * @brief Найти пользователя по логину.
     * @param login Логин пользователя.
     * @return Объект User или невалидный объект, если не найден.
     */
    static User findByLogin(const QString& login);

    /**
     * @brief Создать нового пользователя в БД.
     * @param user Данные пользователя для создания.
     * @return true, если пользователь создан успешно.
     */
    static bool create(const User& user, const QString& passwordHash);

    /**
     * @brief Проверить существование пользователя с данным логином.
     * @param login Логин для проверки.
     * @return true, если пользователь существует.
     */
    static bool existsByLogin(const QString& login);

    /**
     * @brief Получить всех пользователей (для админки).
     * @return Список всех пользователей.
     */
    static QList<User> findAll();

    /**
     * @brief Обновить данные пользователя.
     * @param user Обновленные данные пользователя.
     * @return true, если обновление прошло успешно.
     */
    static bool update(const User& user);

    /**
     * @brief Удалить пользователя по ID.
     * @param userId ID пользователя для удаления.
     * @return true, если удаление прошло успешно.
     */
    static bool deleteById(int userId);
};