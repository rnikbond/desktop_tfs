QT += core gui widgets

CONFIG += c++17

INCLUDEPATH += src

SOURCES += main.cpp \
           src/MainWindow.cpp \
           src/SettingsDialog.cpp

HEADERS += src/MainWindow.h \
    src/SettingsDialog.h \
    src/conf.h

FORMS += src/MainWindow.ui \
    src/SettingsDialog.ui

RESOURCES += \
    res/resources.qrc
