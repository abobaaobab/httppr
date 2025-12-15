#pragma once

#include <QString>
#include "DomainTypes.h"

/**
 * @brief Класс для сохранения и загрузки данных курса.
 * Реализует бинарную сериализацию с простым XOR-шифрованием.
 */
class Serializer {
public:
    /**
     * @brief Сохраняет объект курса в файл.
     *
     * Метод сериализует курс в бинарный формат, шифрует данные методом XOR
     * и записывает результат в указанный файл.
     *
     * @param course Объект курса для сохранения.
     * @param filePath Полный путь к файлу.
     * @throws std::runtime_error Если не удалось открыть файл для записи.
     */
    static void save(const Course& course, const QString& filePath);

    /**
     * @brief Загружает объект курса из файла.
     *
     * Метод считывает зашифрованные данные, расшифровывает их (XOR)
     * и десериализует в структуру Course.
     *
     * @param filePath Полный путь к файлу.
     * @return Загруженный объект Course.
     * @throws std::runtime_error Если файл не открылся, пуст или данные повреждены.
     */
    static Course load(const QString& filePath);

private:
    /// @brief Ключ шифрования (0x5A).
    static const char XOR_KEY = 0x5A;
};
