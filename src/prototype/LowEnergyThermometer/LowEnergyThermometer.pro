# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------
win32 {
TEMPLATE = app
TARGET = CDTT
DESTDIR = ../../../../CypressBuilds/LowEnergyThermometer/x64/Debug
CONFIG += debug
LIBS += -L"."
DEPENDPATH += .
MOC_DIR += .
OBJECTS_DIR += debug
UI_DIR += .
RCC_DIR += GeneratedFiles
}

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets bluetooth

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../auxiliary/CommandLineParser.cpp \
    ../../auxiliary/BluetoothUtil.cpp \
    ../../data/MeasurementBase.cpp \
    ../../data/TemperatureMeasurement.cpp \
    ../../data/TemperatureTest.cpp \
    ../../managers/ManagerBase.cpp \
    ../../managers/BluetoothLEManager.cpp \
    MainWindow.cpp \
    main.cpp

HEADERS += \
    ../../auxiliary/CommandLineParser.h \
    ../../auxiliary/BluetoothUtil.h \
    ../../data/MeasurementBase.h \
    ../../data/TemperatureMeasurement.h \
    ../../data/TemperatureTest.h \
    ../../data/TestBase.h \
    ../../managers/ManagerBase.h \
    ../../managers/BluetoothLEManager.h \
    MainWindow.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
