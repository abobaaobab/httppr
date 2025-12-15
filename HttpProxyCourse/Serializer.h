#pragma once

#include <QString>
#include "DomainTypes.h"

/**
 * @brief Класс для сохранения и загрузки данных курса.
 * Реализует строго бинарную сериализацию через QDataStream согласно ТЗ.
 */
class Serializer {
public:
    /**
     * @brief Сохраняет объект курса в бинарный файл.
     *
     * Метод сериализует курс в чистый бинарный формат через QDataStream
     * без какого-либо шифрования, согласно требованиям ТЗ.
     *
     * @param course Объект курса для сохранения.
     * @param filePath Полный путь к файлу.
     * @throws std::runtime_error Если не удалось открыть файл для записи.
     */
    static void save(const Course& course, const QString& filePath);

    /**
     * @brief Загружает объект курса из бинарного файла.
     *
     * Метод считывает данные через QDataStream и десериализует
     * в структуру Course. Файл не читаем текстовыми редакторами.
     *
     * @param filePath Полный путь к файлу.
     * @return Загруженный объект Course.
     * @throws std::runtime_error Если файл не открылся, пуст или данные повреждены.
     */
    static Course load(const QString& filePath);

    /**
     * @brief Создает начальные данные курса и сохраняет в бинарный файл.
     *
     * Генерирует структуру курса с темами и вопросами согласно ТЗ
     * и сохраняет в бинарном формате.
     *
     * @param filePath Путь для сохранения файла курса.
     * @throws std::runtime_error Если не удалось создать или сохранить данные.
     */
    static void generateCourseData(const QString& filePath);
};
