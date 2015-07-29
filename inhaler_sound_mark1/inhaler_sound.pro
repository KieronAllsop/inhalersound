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
            qt_gui/play_wave.cpp \
    wave/wave_file.cpp \
    wave/show_form.cpp \
    wave/utils.cpp \
    qt_gui/view/explore_patient.cpp \
    qt_gui/prompt/get_patient.cpp \
    qt_gui/prompt/login.cpp


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
    inhaler/patient_wave_details.hpp \
    wave/wave_file.h \
    wave/show_form.h \
    wave/utils.h \
    qt_gui/view/explore_patient.h \
    qt_gui/prompt/get_patient.h \
    qt_gui/prompt/login.h \
    application/state.hpp \
    inhaler/patient_retriever.hpp

FORMS   += \
            qt_gui/administration.ui

OTHER_FILES += \
            initial_data/accuhaler.voc \
            sconscript \
            sconstruct \
    initial_data/test.wav \
    qt_gui/view/test.wav

