#-------------------------------------------------
#
# Project created by QtCreator 2015-06-28T17:52:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = inhalersound
TEMPLATE = app
INCLUDEPATH += $$PWD/.build/gcc49/dbg/working
INCLUDEPATH += $$PWD/.build/gcc49/rel/working

SOURCES += application/main.cpp \
           qt_windows/patientdetails.cpp \
           qt_windows/login.cpp \
           qt_windows/administration.cpp


HEADERS += \
           data_model/schema.hpp \
           qt_windows/patientdetails.h \
           qt_windows/ui_patientdetails.h \
           qt_windows/login.h \
           qt_windows/administration.h

FORMS   += \
           qt_windows/patientdetails.ui \
           qt_windows/login.ui \
           qt_windows/administration.ui
