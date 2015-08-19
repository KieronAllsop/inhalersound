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
            qt_gui/login.cpp \
            qt_gui/mainwindow.cpp \
            qt_gui/view/explore_patient.cpp \
            qt_gui/prompt/get_patient.cpp \
            qt_gui/prompt/login.cpp \
            qt_gui/view/wave_form.cpp \
            qt_gui/view/explore_wave.cpp \
            qt/audio/audio_buffer_test.cpp \
    qt_gui/view/wave_zoom_start.cpp \
    qt_gui/view/wave_zoom_end.cpp

HEADERS += \
            data_model/schema.hpp \
            inhaler/server.hpp \
            inhaler/wave_importer.hpp \
            inhaler/wave_details.hpp \
            inhaler/data_retriever.hpp \
            inhaler/patient_wave_details.hpp \
            inhaler/patient_retriever.hpp \
            qt_gui/import_wizard/get_patient_page.h \
            qt_gui/import_wizard/intro_page.h \
            qt_gui/import_wizard/process_files_page.h \
            qt_gui/import_wizard/select_files_page.h \
            qt_gui/import_wizard/wizard.h \
            qt_gui/login.h \
            qt_gui/mainwindow.h \
            qt_gui/view/explore_patient.h \
            qt_gui/prompt/get_patient.h \
            qt_gui/prompt/login.h \
            qt/audio/audio_decoder.hpp \
            qt/audio/decode_buffer.hpp \
            qt/audio/decode_status.hpp \
            qt/audio/format.hpp \
            qt/audio/raw_data.hpp \
            qt/audio/wave_decoder.hpp \
            qt_gui/view/wave_form.h \
            qt_gui/view/explore_wave.h \
            qt/audio/audio_buffer.hpp \
            qt/audio/audio_player.hpp \
            qt/audio/play_status.hpp \
            qt/audio/probe_status.hpp \
    qt_gui/view/wave_zoom_start.h \
    qt_gui/view/wave_zoom_end.h \
    qt/audio/vocabulary_kind.hpp

FORMS   +=

OTHER_FILES += \
            initial_data/accuhaler.voc \
            sconscript \
            sconstruct \
            qt/audio/audio_buffer_test \
    initial_data/inhaler.spe
