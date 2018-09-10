#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T06:48:31
#
#-------------------------------------------------

QT       += core gui
#QT += charts

DEFINES += QWT_DLL
DEFINES += QWT_DLL
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/3rd_party/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rd_party/lib/ -lqwtd

INCLUDEPATH += $$PWD/3rd_party/include
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ECGChart
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ecgchart.cpp \

HEADERS  += mainwindow.h \
    ecgchart.h \

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
