#include "Seeder.h"
#include "DomainTypes.h"
#include "Serializer.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QVariant>

void Seeder::generate(const QString& jsonPath, const QString& binPath) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[Seeder] Error: Could not open JSON file at:" << jsonPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "[Seeder] JSON Parse Error:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qDebug() << "[Seeder] Error: Root JSON element is not an Object.";
        return;
    }

    Course course;
    QJsonObject root = doc.object();

    if (root.contains("topics") && root["topics"].isArray()) {
        QJsonArray topicsArray = root["topics"].toArray();

        for (const QJsonValue& topicVal : topicsArray) {
            if (!topicVal.isObject()) continue;

            QJsonObject topicObj = topicVal.toObject();
            Topic topic;

            topic.title = topicObj.value("title").toString("Untitled Topic");
            topic.htmlContent = topicObj.value("htmlContent").toString("<p>No content</p>");

            if (topicObj.contains("questions") && topicObj["questions"].isArray()) {
                QJsonArray questionsArray = topicObj["questions"].toArray();

                for (const QJsonValue& qVal : questionsArray) {
                    if (!qVal.isObject()) continue;

                    QJsonObject qObj = qVal.toObject();
                    Question question;

                    // Безопасное чтение полей с проверкой наличия
                    question.text = qObj.contains("text") ? qObj["text"].toString("Empty Question") : "Empty Question";
                    question.correctIndex = qObj.contains("correctIndex") ? qObj["correctIndex"].toInt(0) : 0;

                    if (qObj.contains("variants") && qObj["variants"].isArray()) {
                        QJsonArray variantsArray = qObj["variants"].toArray();
                        for (const QJsonValue& vVal : variantsArray) {
                            question.variants.append(vVal.toString("Empty Variant"));
                        }
                    }

                    if (question.variants.isEmpty()) {
                        question.variants << "Yes" << "No";
                    }

                    // Критическая валидация correctIndex после заполнения variants
                    if (question.correctIndex < 0 || question.correctIndex >= question.variants.size()) {
                        qWarning() << "[Seeder] Invalid correctIndex" << question.correctIndex 
                                  << "for question with" << question.variants.size() << "variants. Reset to 0.";
                        question.correctIndex = 0;
                    }

                    topic.questions.append(question);
                }
            }

            course.topics.append(topic);
        }
    } else {
        qDebug() << "[Seeder] Warning: No 'topics' array found in JSON.";
    }

    try {
        Serializer::save(course, binPath);
        qDebug() << "[Seeder] Success: Course generated and saved to" << binPath;
    } catch (const std::exception& e) {
        qDebug() << "[Seeder] Critical Error during serialization:" << e.what();
    }
}
