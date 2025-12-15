#pragma once

#include <QString>

/*!
 * @brief Класс для инициализации данных курса.
 * 
 * Отвечает за первичную инициализацию данных курса из JSON-файла
 * и конвертацию их в защищенный бинарный формат с использованием
 * сериализации QDataStream.
 */
class Seeder {
public:
    /**
     * @brief Считывает JSON, формирует структуру Course и сохраняет в бинарный файл.
     * @param jsonPath Путь к исходному файлу source.json.
     * @param binPath Путь, куда сохранить зашифрованный файл курса.
     */
    static void generate(const QString& jsonPath, const QString& binPath);
};
