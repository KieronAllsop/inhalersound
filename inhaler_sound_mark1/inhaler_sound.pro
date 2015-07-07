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
           qt_windows/mainwindow.cpp \
           qt_windows/patientdetails.cpp


HEADERS += qt_windows/mainwindow.h \
           data_model/schema.hpp \
           qt_windows/patientdetails.h

FORMS   += qt_windows/mainwindow.ui \
           qt_windows/patientdetails.ui
