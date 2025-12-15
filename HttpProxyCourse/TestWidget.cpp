#include "TestWidget.h"
#include "TestResultDao.h"
#include "DatabaseManager.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

TestWidget::TestWidget(QWidget *parent) 
    : QWidget(parent)
    , m_currentQuestionIndex(0)
    , m_correctAnswers(0)
    , m_timeLimitMinutes(20)
    , m_testActive(false)
{
    auto mainLayout = new QVBoxLayout(this);

    // Создание панели статуса
    auto statusLayout = new QHBoxLayout();
    
    m_timeLabel = new QLabel("Время: --:--", this);
    m_timeLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    
    m_progressLabel = new QLabel("Вопрос: 0/0", this);
    
    statusLayout->addWidget(m_timeLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_progressLabel);
    
    // Прогресс-бар
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);

    // Основной контент
    questionLabel = new QLabel("Загрузка...", this);
    questionLabel->setWordWrap(true);
    QFont f = questionLabel->font();
    f.setPointSize(12);
    f.setBold(true);
    questionLabel->setFont(f);

    variantsContainer = new QWidget(this);
    variantsLayout = new QVBoxLayout(variantsContainer);
    btnAnswer = new QPushButton("Ответить", this);

    mainLayout->addLayout(statusLayout);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(variantsContainer);
    mainLayout->addWidget(btnAnswer);
    mainLayout->addStretch();

    // Инициализация таймеров
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &TestWidget::onTimeExpired);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &TestWidget::updateTimer);

    // Подключение сигналов
    connect(btnAnswer, &QPushButton::clicked, this, &TestWidget::onAnswerSubmitted);
}

void TestWidget::showQuestion(const Question& question) {
    // Безопасная очистка layout с проверкой родительского виджета
    while (QLayoutItem* item = variantsLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            // Дополнительная проверка принадлежности виджета контейнеру
            if (widget->parent() == variantsContainer) {
                widget->deleteLater();
            }
        }
        delete item;
    }
    radioButtons.clear();

    questionLabel->setText(question.text);

    for (const QString& variant : question.variants) {
        auto rb = new QRadioButton(variant, variantsContainer);
        variantsLayout->addWidget(rb);
        radioButtons.append(rb);
    }
}

int TestWidget::getSelectedVariantIndex() {
    for (int i = 0; i < radioButtons.size(); ++i) {
        if (radioButtons[i]->isChecked()) {
            return i;
        }
    }
    return -1;
}

void TestWidget::startTest(const QList<Question>& questions, const User& user, int timeLimit) {
    m_questions = questions;
    m_currentUser = user;
    m_timeLimitMinutes = timeLimit;
    m_currentQuestionIndex = 0;
    m_correctAnswers = 0;
    m_testActive = true;
    m_testStartTime = QDateTime::currentDateTime();

    // Настройка прогресс-бара
    m_progressBar->setMaximum(m_questions.size());
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);

    // Запуск таймеров
    m_timer->start(m_timeLimitMinutes * 60 * 1000); // Конвертация в миллисекунды
    m_updateTimer->start(1000); // Обновление каждую секунду

    updateProgress();
    showNextQuestion();

    qDebug() << "Test started for user" << user.login << "with" << questions.size() << "questions";
}

void TestWidget::finishTest() {
    if (!m_testActive) {
        return;
    }

    m_testActive = false;
    m_timer->stop();
    m_updateTimer->stop();

    bool timeExpired = m_timer->remainingTime() <= 0;
    
    // Сохранение результата в БД
    saveTestResult();

    // Отправка сигнала о завершении
    emit testFinished(m_correctAnswers, m_questions.size(), timeExpired);

    qDebug() << "Test finished. Score:" << m_correctAnswers << "/" << m_questions.size();
}

int TestWidget::getCurrentScore() const {
    return m_correctAnswers;
}

int TestWidget::getMaxScore() const {
    return m_questions.size();
}

void TestWidget::onTimeExpired() {
    QMessageBox::warning(this, "Время истекло", 
                        QString("Время тестирования (%1 мин) истекло!\nТест будет завершен автоматически.")
                        .arg(m_timeLimitMinutes));
    finishTest();
}

void TestWidget::updateTimer() {
    if (!m_testActive) {
        return;
    }

    int remainingMs = m_timer->remainingTime();
    int remainingSeconds = remainingMs / 1000;
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;

    m_timeLabel->setText(QString("Время: %1:%2")
                        .arg(minutes, 2, 10, QChar('0'))
                        .arg(seconds, 2, 10, QChar('0')));

    // Изменение цвета при малом времени
    if (remainingSeconds < 300) { // Менее 5 минут
        m_timeLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    } else if (remainingSeconds < 600) { // Менее 10 минут
        m_timeLabel->setStyleSheet("QLabel { color: orange; font-weight: bold; }");
    } else {
        m_timeLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    }
}

void TestWidget::onAnswerSubmitted() {
    if (!m_testActive) {
        return;
    }

    int selectedIndex = getSelectedVariantIndex();
    if (selectedIndex == -1) {
        QMessageBox::warning(this, "Внимание", "Пожалуйста, выберите вариант ответа.");
        return;
    }

    // Проверка правильности ответа
    const Question& currentQuestion = m_questions[m_currentQuestionIndex];
    if (selectedIndex == currentQuestion.correctIndex) {
        m_correctAnswers++;
    }

    m_currentQuestionIndex++;
    updateProgress();

    if (m_currentQuestionIndex >= m_questions.size()) {
        finishTest();
    } else {
        showNextQuestion();
    }

    // Отправка сигнала для совместимости
    emit answerSubmitted(selectedIndex);
}

bool TestWidget::saveTestResult() {
    if (!m_currentUser.isValid()) {
        qWarning() << "Cannot save test result: invalid user";
        return false;
    }

    // Создание объекта результата теста
    TestResult result(m_currentUser.id, m_correctAnswers, m_questions.size());
    
    // Сохранение через DAO
    if (!TestResultDao::save(result)) {
        qCritical() << "Failed to save test result through DAO";
        return false;
    }

    qDebug() << "Test result saved for user" << m_currentUser.login 
             << "score:" << m_correctAnswers << "/" << m_questions.size();
    return true;
}

void TestWidget::updateProgress() {
    m_progressLabel->setText(QString("Вопрос: %1/%2")
                            .arg(m_currentQuestionIndex + 1)
                            .arg(m_questions.size()));
    
    m_progressBar->setValue(m_currentQuestionIndex);
}

void TestWidget::showNextQuestion() {
    if (m_currentQuestionIndex < m_questions.size()) {
        showQuestion(m_questions[m_currentQuestionIndex]);
        
        // Обновление текста кнопки для последнего вопроса
        if (m_currentQuestionIndex == m_questions.size() - 1) {
            btnAnswer->setText("Завершить тест");
        } else {
            btnAnswer->setText("Следующий вопрос");
        }
    }
}
