#-------------------------------------------------
#
# Project created by QtCreator 2015-06-28T17:52:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = inhalersound
TEMPLATE = app


SOURCES += application/main.cpp \
           qt_windows/patientdetails.cpp \
    qt_windows/login.cpp


HEADERS += \
           data_model/schema.hpp \
           qt_windows/patientdetails.h \
    qt_windows/login.h

FORMS   += \
           qt_windows/patientdetails.ui \
    qt_windows/login.ui
