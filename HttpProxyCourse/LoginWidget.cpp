#include "LoginWidget.h"
#include "AuthService.h"
#include <QFont>
#include <QFormLayout>
#include <QGridLayout>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent) {
    // Создание основного layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Заголовок
    QLabel* welcomeLabel = new QLabel("Добро пожаловать в курс HTTP Proxy", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = welcomeLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    welcomeLabel->setFont(titleFont);
    
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addSpacing(20);
    
    // Создание вкладок
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createLoginTab(), "Вход");
    m_tabWidget->addTab(createRegisterTab(), "Регистрация");
    m_tabWidget->addTab(createQuickAccessTab(), "Быстрый доступ");
    
    mainLayout->addWidget(m_tabWidget);
    mainLayout->setContentsMargins(50, 20, 50, 20);
}

QWidget* LoginWidget::createLoginTab() {
    QWidget* loginTab = new QWidget();
    QFormLayout* layout = new QFormLayout(loginTab);
    
    m_loginEdit = new QLineEdit();
    m_loginEdit->setPlaceholderText("Введите логин");
    // Валидация логина: только латинские буквы, цифры и подчеркивание, 3-20 символов
    QRegularExpressionValidator* loginValidator = new QRegularExpressionValidator(
        QRegularExpression("^[a-zA-Z0-9_]{3,20}$"), m_loginEdit);
    m_loginEdit->setValidator(loginValidator);
    
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText("Введите пароль");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    // Валидация пароля: минимум 4 символа
    QRegularExpressionValidator* passwordValidator = new QRegularExpressionValidator(
        QRegularExpression("^.{4,}$"), m_passwordEdit);
    m_passwordEdit->setValidator(passwordValidator);
    
    m_loginButton = new QPushButton("Войти");
    
    layout->addRow("Логин:", m_loginEdit);
    layout->addRow("Пароль:", m_passwordEdit);
    layout->addRow("", m_loginButton);
    
    // Подключение сигналов
    connect(m_loginButton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLoginClicked);
    
    return loginTab;
}

QWidget* LoginWidget::createRegisterTab() {
    QWidget* registerTab = new QWidget();
    QFormLayout* layout = new QFormLayout(registerTab);
    
    m_regLoginEdit = new QLineEdit();
    m_regLoginEdit->setPlaceholderText("Выберите логин");
    // Валидация логина для регистрации: только латинские буквы, цифры и подчеркивание, 3-20 символов
    QRegularExpressionValidator* regLoginValidator = new QRegularExpressionValidator(
        QRegularExpression("^[a-zA-Z0-9_]{3,20}$"), m_regLoginEdit);
    m_regLoginEdit->setValidator(regLoginValidator);
    
    m_regPasswordEdit = new QLineEdit();
    m_regPasswordEdit->setPlaceholderText("Выберите пароль");
    m_regPasswordEdit->setEchoMode(QLineEdit::Password);
    // Валидация пароля для регистрации: минимум 4 символа
    QRegularExpressionValidator* regPasswordValidator = new QRegularExpressionValidator(
        QRegularExpression("^.{4,}$"), m_regPasswordEdit);
    m_regPasswordEdit->setValidator(regPasswordValidator);
    
    m_regFullNameEdit = new QLineEdit();
    m_regFullNameEdit->setPlaceholderText("Ваше полное имя");
    // Валидация полного имени: только буквы, пробелы и дефисы, 2-50 символов
    QRegularExpressionValidator* nameValidator = new QRegularExpressionValidator(
        QRegularExpression("^[a-zA-Zа-яА-Я\\s\\-]{2,50}$"), m_regFullNameEdit);
    m_regFullNameEdit->setValidator(nameValidator);
    
    m_registerButton = new QPushButton("Зарегистрироваться");
    
    layout->addRow("Логин:", m_regLoginEdit);
    layout->addRow("Пароль:", m_regPasswordEdit);
    layout->addRow("Полное имя:", m_regFullNameEdit);
    layout->addRow("", m_registerButton);
    
    // Подключение сигналов
    connect(m_registerButton, &QPushButton::clicked, this, &LoginWidget::onRegisterClicked);
    
    return registerTab;
}

QWidget* LoginWidget::createQuickAccessTab() {
    QWidget* quickTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(quickTab);
    
    layout->addStretch();
    
    m_studentStartButton = new QPushButton("Начать обучение (Гость)");
    m_studentStartButton->setMinimumHeight(40);
    
    layout->addWidget(m_studentStartButton);
    layout->addSpacing(20);
    
    QLabel* adminLabel = new QLabel("Администратор:");
    m_adminPasswordEdit = new QLineEdit();
    m_adminPasswordEdit->setPlaceholderText("Пароль администратора");
    m_adminPasswordEdit->setEchoMode(QLineEdit::Password);
    
    m_adminLoginButton = new QPushButton("Войти как Администратор");
    
    layout->addWidget(adminLabel);
    layout->addWidget(m_adminPasswordEdit);
    layout->addWidget(m_adminLoginButton);
    layout->addStretch();
    
    // Подключение сигналов для обратной совместимости
    connect(m_adminLoginButton, &QPushButton::clicked, this, &LoginWidget::onAdminLoginClicked);
    connect(m_studentStartButton, &QPushButton::clicked, this, &LoginWidget::startStudentSession);
    connect(m_adminPasswordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onAdminLoginClicked);
    
    return quickTab;
}

void LoginWidget::onLoginClicked() {
    QString login = m_loginEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    
    if (!validateInput(login, password)) {
        return;
    }
    
    User user;
    AuthService::AuthResult result = AuthService::login(login, password, user);
    
    switch (result) {
        case AuthService::AuthResult::Success:
            showSuccess(QString("Добро пожаловать, %1!").arg(user.fullName));
            emit userAuthenticated(user);
            break;
            
        case AuthService::AuthResult::UserNotFound:
            showError("Пользователь не найден. Проверьте логин или зарегистрируйтесь.");
            break;
            
        case AuthService::AuthResult::InvalidCredentials:
            showError("Неверный пароль. Попробуйте еще раз.");
            m_passwordEdit->clear();
            m_passwordEdit->setFocus();
            break;
            
        case AuthService::AuthResult::DatabaseError:
            showError("Ошибка подключения к базе данных. Попробуйте позже.");
            break;
    }
}

void LoginWidget::onRegisterClicked() {
    QString login = m_regLoginEdit->text().trimmed();
    QString password = m_regPasswordEdit->text();
    QString fullName = m_regFullNameEdit->text().trimmed();
    
    if (!validateInput(login, password, fullName)) {
        return;
    }
    
    AuthService::RegisterResult result = AuthService::registerUser(login, password, fullName, "student");
    
    switch (result) {
        case AuthService::RegisterResult::Success:
            showSuccess("Регистрация прошла успешно! Теперь вы можете войти в систему.");
            m_regLoginEdit->clear();
            m_regPasswordEdit->clear();
            m_regFullNameEdit->clear();
            m_tabWidget->setCurrentIndex(0); // Переключиться на вкладку входа
            break;
            
        case AuthService::RegisterResult::UserExists:
            showError("Пользователь с таким логином уже существует. Выберите другой логин.");
            m_regLoginEdit->setFocus();
            break;
            
        case AuthService::RegisterResult::InvalidInput:
            showError("Некорректные данные. Проверьте заполнение всех полей.");
            break;
            
        case AuthService::RegisterResult::DatabaseError:
            showError("Ошибка базы данных. Попробуйте позже.");
            break;
    }
}

void LoginWidget::onAdminLoginClicked() {
    emit adminLoginAttempt(m_adminPasswordEdit->text());
    m_adminPasswordEdit->clear();
}

bool LoginWidget::validateInput(const QString& login, const QString& password, const QString& fullName) {
    if (login.isEmpty()) {
        showError("Логин не может быть пустым.");
        return false;
    }
    
    if (login.length() < 3) {
        showError("Логин должен содержать минимум 3 символа.");
        return false;
    }
    
    if (password.isEmpty()) {
        showError("Пароль не может быть пустым.");
        return false;
    }
    
    if (password.length() < 4) {
        showError("Пароль должен содержать минимум 4 символа.");
        return false;
    }
    
    if (!fullName.isEmpty() && fullName.length() < 2) {
        showError("Полное имя должно содержать минимум 2 символа.");
        return false;
    }
    
    return true;
}

void LoginWidget::showError(const QString& message) {
    QMessageBox::warning(this, "Ошибка", message);
}

void LoginWidget::showSuccess(const QString& message) {
    QMessageBox::information(this, "Успех", message);
}
