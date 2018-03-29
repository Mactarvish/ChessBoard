#-------------------------------------------------
#
# Project created by QtCreator 2016-03-28T08:44:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chessboard
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

DISTFILES +=

RESOURCES += \
    chesssource.qrc
CONFIG += c++11
