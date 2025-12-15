#include "CourseDataConverter.h"
#include "Serializer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

bool CourseDataConverter::convertJsonToBinary(const QString& jsonFilePath, const QString& binaryFilePath) {
    // Парсим JSON файл
    Course course = parseJsonFile(jsonFilePath);
    
    if (course.topics.isEmpty()) {
        qCritical() << "Failed to parse JSON file or course is empty:" << jsonFilePath;
        return false;
    }
    
    try {
        // Сохраняем в бинарном формате
        Serializer::save(course, binaryFilePath);
        qDebug() << "Successfully converted" << course.topics.size() << "topics from JSON to binary format";
        return true;
    } catch (const std::exception& e) {
        qCritical() << "Failed to save binary file:" << e.what();
        return false;
    }
}

Course CourseDataConverter::parseJsonFile(const QString& jsonFilePath) {
    Course course;
    
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open JSON file:" << jsonFilePath;
        return course;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << "JSON parse error:" << parseError.errorString();
        return course;
    }
    
    QJsonObject rootObj = doc.object();
    QJsonArray topicsArray = rootObj["topics"].toArray();
    
    for (const QJsonValue& topicValue : topicsArray) {
        QJsonObject topicObj = topicValue.toObject();
        
        Topic topic;
        topic.title = topicObj["title"].toString();
        topic.htmlContent = topicObj["htmlContent"].toString();
        
        QJsonArray questionsArray = topicObj["questions"].toArray();
        for (const QJsonValue& questionValue : questionsArray) {
            QJsonObject questionObj = questionValue.toObject();
            
            Question question;
            question.text = questionObj["text"].toString();
            question.correctIndex = questionObj["correctIndex"].toInt();
            
            QJsonArray variantsArray = questionObj["variants"].toArray();
            for (const QJsonValue& variantValue : variantsArray) {
                question.variants.append(variantValue.toString());
            }
            
            topic.questions.append(question);
        }
        
        course.topics.append(topic);
    }
    
    qDebug() << "Parsed" << course.topics.size() << "topics from JSON";
    return course;
}