#-------------------------------------------------
#
# Project created by QtCreator 2019-04-17T12:03:18
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MQTT_CLIENT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    mainwindow_slots.cpp \
    mainwindow_ui.cpp

HEADERS += \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/mqtt/lib/ -lqmqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/mqtt/lib/ -lqmqttd
else:unix: LIBS += -L$$PWD/mqtt/lib/ -lqmqtt

INCLUDEPATH += $$PWD/mqtt/include
DEPENDPATH += $$PWD/mqtt/include

RESOURCES += \
    res.qrc

RC_FILE = logo.rc
