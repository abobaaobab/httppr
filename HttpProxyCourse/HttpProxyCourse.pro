QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AdminWidget.cpp \
    AuthService.cpp \
    DatabaseConfig.cpp \
    DatabaseManager.cpp \
    Logger.cpp \
    LoginWidget.cpp \
    Seeder.cpp \
    Serializer.cpp \
    SessionManager.cpp \
    StudentProfileWidget.cpp \
    TestWidget.cpp \
    TopicSelectionWidget.cpp \
    TopicViewWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AdminWidget.h \
    AuthService.h \
    DatabaseConfig.h \
    DatabaseManager.h \
    DomainTypes.h \
    Logger.h \
    LoginWidget.h \
    Seeder.h \
    Serializer.h \
    SessionManager.h \
    StudentProfileWidget.h \
    TestWidget.h \
    TopicSelectionWidget.h \
    TopicViewWidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    source.json

RESOURCES += \
    resources.qrc
