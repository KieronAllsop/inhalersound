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

SOURCES +=  application/main.cpp \
            qt_windows/play_wave.cpp \
            qt_windows/mainwindow.cpp \
            qt_gui/import_wizard/get_patient_page.cpp \
            qt_gui/import_wizard/intro_page.cpp \
            qt_gui/import_wizard/process_files_page.cpp \
            qt_gui/import_wizard/select_files_page.cpp \
            qt_gui/import_wizard/wizard.cpp \
            qt_gui/administration.cpp \
            qt_gui/login.cpp \
            qt_gui/mainwindow.cpp \
            qt_gui/play_wave.cpp


HEADERS += \
            data_model/schema.hpp \
            inhaler/server.hpp \
            inhaler/wave_importer.hpp \
            inhaler/wave_details.hpp \
            qt_gui/import_wizard/get_patient_page.h \
            qt_gui/import_wizard/intro_page.h \
            qt_gui/import_wizard/process_files_page.h \
            qt_gui/import_wizard/select_files_page.h \
            qt_gui/import_wizard/wizard.h \
            qt_gui/administration.h \
            qt_gui/login.h \
            qt_gui/mainwindow.h \
            qt_gui/play_wave.h \
    inhaler/data_retriever.hpp \
    inhaler/patient_wave_details.hpp

FORMS   += \
            qt_gui/administration.ui

OTHER_FILES += \
            initial_data/accuhaler.voc \
            sconscript \
            sconstruct

