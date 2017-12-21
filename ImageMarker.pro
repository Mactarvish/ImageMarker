#-------------------------------------------------
#
# Project created by QtCreator 2017-09-08T16:41:46
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageMarker
TEMPLATE = app


SOURCES += main.cpp\
        ImageMarker.cpp \
    database.cpp

HEADERS  += ImageMarker.h \
    database.h

FORMS    += ImageMarker.ui
