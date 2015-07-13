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
           qt_windows/administration.cpp \
           qt_windows/process_sounds_import_files.cpp \
           qt_windows/process_sounds_intro_page.cpp \
           qt_windows/process_sounds_confirm_files.cpp \
    qt_windows/process_sounds_get_patient_page.cpp


HEADERS += \
           data_model/schema.hpp \
           qt_windows/patientdetails.h \
           qt_windows/ui_patientdetails.h \
           qt_windows/login.h \
           qt_windows/administration.h \
           qt_windows/process_sounds_import_files.h \
           qt_windows/process_sounds_intro_page.h \
           qt_windows/process_sounds_confirm_files.h \
    qt_windows/process_sounds_get_patient_page.h

FORMS   += \
           qt_windows/login.ui \
           qt_windows/administration.ui
