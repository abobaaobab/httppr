#include "Serializer.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <stdexcept>

void Serializer::save(const Course& course, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Cannot open file for writing: " + filePath.toStdString());
    }

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_15);

    // Прямая сериализация объекта курса в бинарный формат
    stream << course;

    if (stream.status() != QDataStream::Ok) {
        throw std::runtime_error("Failed to serialize course data to: " + filePath.toStdString());
    }
    
    qDebug() << "Course data saved to" << filePath << "(" << file.size() << "bytes)";
}

Course Serializer::load(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Cannot open file for reading: " + filePath.toStdString());
    }

    if (file.size() == 0) {
        throw std::runtime_error("File is empty: " + filePath.toStdString());
    }

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_5_15);

    // Прямая десериализация объекта курса из бинарного формата
    Course course;
    stream >> course;

    if (stream.status() != QDataStream::Ok) {
        throw std::runtime_error("Failed to deserialize course data from: " + filePath.toStdString());
    }

    qDebug() << "Course data loaded from" << filePath << "(" << course.topics.size() << "topics)";
    return course;
}

void Serializer::generateCourseData(const QString& filePath) {
    Course course;
    
    // Тема 1: Введение в HTTP Proxy
    Topic topic1;
    topic1.title = "Введение в HTTP Proxy";
    topic1.htmlContent = R"(
        <h2>HTTP Прокси-сервер</h2>
        <p><b>HTTP Прокси-сервер</b> — это промежуточный узел (посредник) в компьютерной сети, который выступает в роли шлюза между пользователем (клиентом) и интернетом (целевым сервером).</p>
        
        <h3>Типы прокси-серверов:</h3>
        <ul>
            <li><b>Forward Proxy (Прямой прокси):</b> Работает в интересах клиента. Обычно используется для обхода ограничений доступа, сокрытия реального IP-адреса пользователя или контроля исходящего трафика в корпоративных сетях.</li>
            <li><b>Reverse Proxy (Обратный прокси):</b> Работает в интересах сервера. Он принимает запросы из интернета и передает их на внутренние серверы, обеспечивая защиту от атак, балансировку нагрузки и SSL-шифрование.</li>
        </ul>
        
        <h3>Преимущества использования прокси:</h3>
        <ul>
            <li><b>Кэширование:</b> Сохранение копий часто запрашиваемых ресурсов для ускорения загрузки и экономии трафика.</li>
            <li><b>Анонимность:</b> Сокрытие информации о клиенте от целевого ресурса.</li>
            <li><b>Фильтрация и контроль:</b> Блокировка нежелательных сайтов, рекламы или вредоносного ПО.</li>
        </ul>
    )";
    
    Question q1;
    q1.text = "Какое определение наиболее точно описывает роль HTTP прокси-сервера?";
    q1.variants << "Это база данных для хранения учетных записей пользователей."
                << "Это посредник между клиентом и сервером, пересылающий запросы и ответы."
                << "Это протокол для прямой передачи файлов между компьютерами."
                << "Это антивирусная программа для защиты рабочего стола.";
    q1.correctIndex = 1;
    
    Question q2;
    q2.text = "В чем заключается основная функция Reverse Proxy (Обратного прокси)?";
    q2.variants << "Защита внутренних серверов и распределение нагрузки (балансировка)."
                << "Помощь пользователю в обходе блокировок провайдера."
                << "Ускорение работы локальной сети принтеров."
                << "Генерация паролей для администраторов.";
    q2.correctIndex = 0;
    
    Question q3;
    q3.text = "Какой механизм прокси-сервера позволяет уменьшить потребление трафика и ускорить загрузку страниц?";
    q3.variants << "Шифрование данных (SSL)."
                << "Кэширование часто запрашиваемых ресурсов."
                << "Фильтрация вредоносного ПО."
                << "Балансировка нагрузки между серверами.";
    q3.correctIndex = 1;
    
    topic1.questions << q1 << q2 << q3;
    
    // Тема 2: Настройка и конфигурация
    Topic topic2;
    topic2.title = "Настройка и конфигурация HTTP Proxy";
    topic2.htmlContent = R"(
        <h2>Настройка HTTP Proxy</h2>
        <p>Правильная настройка прокси-сервера критически важна для обеспечения безопасности и производительности сети.</p>
        
        <h3>Основные параметры конфигурации:</h3>
        <ul>
            <li><b>Порт прослушивания:</b> Обычно 8080, 3128 или 8888</li>
            <li><b>Методы аутентификации:</b> Basic, Digest, NTLM</li>
            <li><b>Правила доступа:</b> ACL (Access Control Lists)</li>
            <li><b>Логирование:</b> Детальная запись всех запросов</li>
        </ul>
        
        <h3>Безопасность:</h3>
        <p>Важно настроить фильтрацию по IP-адресам, доменам и типам контента для предотвращения несанкционированного доступа.</p>
    )";
    
    Question q4;
    q4.text = "Какой порт чаще всего используется для HTTP прокси-серверов?";
    q4.variants << "80" << "443" << "8080" << "22";
    q4.correctIndex = 2;
    
    Question q5;
    q5.text = "Что такое ACL в контексте прокси-серверов?";
    q5.variants << "Automatic Cache Loading"
                << "Access Control Lists (Списки контроля доступа)"
                << "Advanced Connection Logic"
                << "Anonymous Client Login";
    q5.correctIndex = 1;
    
    topic2.questions << q4 << q5;
    
    course.topics << topic1 << topic2;
    
    // Сохраняем сгенерированные данные
    save(course, filePath);
    qDebug() << "Generated course data with" << course.topics.size() << "topics";
}