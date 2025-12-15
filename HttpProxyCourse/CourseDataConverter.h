#pragma once

#include <QString>
#include "DomainTypes.h"

/**
 * @brief Утилита для конвертации данных курса из JSON в бинарный формат.
 * 
 * Этот класс используется для одноразовой конвертации существующих JSON данных
 * в требуемый бинарный формат с использованием QDataStream.
 */
class CourseDataConverter {
public:
    /**
     * @brief Конвертирует JSON файл курса в бинарный формат.
     * @param jsonFilePath Путь к исходному JSON файлу.
     * @param binaryFilePath Путь для сохранения бинарного файла.
     * @return true, если конвертация прошла успешно.
     */
    static bool convertJsonToBinary(const QString& jsonFilePath, const QString& binaryFilePath);

private:
    /**
     * @brief Парсит JSON и создает объект Course.
     * @param jsonFilePath Путь к JSON файлу.
     * @return Объект Course или пустой объект при ошибке.
     */
    static Course parseJsonFile(const QString& jsonFilePath);
};