QT += core gui widgets

CONFIG += c++17

INCLUDEPATH += src

SOURCES += main.cpp \
           src/AzureConnectDialog.cpp \
           src/MainWindow.cpp \
           src/ManagerTFS.cpp \
           src/SettingsDialog.cpp \
           src/SettingsDialog_config.cpp

HEADERS += src/MainWindow.h \
    src/AzureConnectDialog.h \
    src/ManagerTFS.h \
    src/SettingsDialog.h \
    src/conf.h

FORMS += src/MainWindow.ui \
    src/AzureConnectDialog.ui \
    src/SettingsDialog.ui

RESOURCES += \
    res/resources.qrc
