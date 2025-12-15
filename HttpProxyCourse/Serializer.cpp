#include "Serializer.h"
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <stdexcept>

void Serializer::save(const Course& course, const QString& filePath) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    stream << course;

    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ XOR_KEY;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Serializer::save - Cannot open file for writing: " + filePath.toStdString());
    }

    file.write(data);
    file.close();
}

Course Serializer::load(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Serializer::load - Cannot open file for reading: " + filePath.toStdString());
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        throw std::runtime_error("Serializer::load - File is empty: " + filePath.toStdString());
    }

    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ XOR_KEY;
    }

    QDataStream stream(&data, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_15);

    Course course;
    stream >> course;

    // Критическая проверка целостности данных после десериализации
    if (stream.status() != QDataStream::Ok) {
        throw std::runtime_error("Serializer::load - Data corruption or version mismatch in: " + filePath.toStdString());
    }

    // Дополнительная проверка: убеждаемся, что все данные прочитаны корректно
    if (stream.atEnd() == false && stream.status() == QDataStream::Ok) {
        qWarning() << "Serializer::load - Warning: Extra data found in file, possible version mismatch";
    }

    return course;
}
