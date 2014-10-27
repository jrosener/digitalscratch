#-------------------------------------------------
#
# Project created by QtCreator 2010-11-08T23:36:00
#
#-------------------------------------------------

# Release version number.
#VERSION = 1.5.0

# Snapshot version number.
win32 {
    VERSION = 1.6.0b
}
unix {
    CURRENT_DATE = $$system(date +%Y%m%d)
    VERSION = 1.5.0+1.6.0SNAPSHOT$${CURRENT_DATE}
}
DEFINES += VERSION=$${VERSION}

CONFIG += qt c++11
INCLUDEPATH += src/include

CONFIG(test) {
    QT       += testlib
    TARGET    = libdigitalscratch-test
    CONFIG   += console
    CONFIG   -= app_bundle
}
else {
    QT -= gui

    TARGET = digitalscratch
    TEMPLATE = lib

    DEFINES += DIGITALSCRATCH_LIBRARY

    target.path = /usr/lib

    include.path = /usr/include
    include.files = src/include/digital_scratch_api.h

    INSTALLS += target include
}

SOURCES += \ 
    src/volume.cpp \
    src/speed.cpp \
    src/serato_vinyl.cpp \
    src/playing_parameter.cpp \
    src/final_scratch_vinyl.cpp \
    src/digital_scratch_api.cpp \
    src/digital_scratch.cpp \
    src/controller.cpp \
    src/coded_vinyl.cpp \
    src/mixvibes_vinyl.cpp \
    src/log.cpp

HEADERS += \ 
    src/include/volume.h \
    src/include/speed.h \
    src/include/serato_vinyl.h \
    src/include/playing_parameter.h \
    src/include/final_scratch_vinyl.h \
    src/include/dscratch_parameters.h \
    src/include/digital_scratch_api.h \
    src/include/digital_scratch.h \
    src/include/controller.h \
    src/include/coded_vinyl.h \
    src/include/mixvibes_vinyl.h \
    src/include/log.h

CONFIG(test) {
    INCLUDEPATH += test

    SOURCES += test/main_test.cpp \
               test/test_utils.cpp \
               test/controller_test.cpp \
               test/digital_scratch_api_test.cpp \
               test/speed_test.cpp \
               test/digital_scratch_test.cpp \
               test/volume_test.cpp

    HEADERS += test/test_utils.h \
               test/controller_test.h \
               test/digital_scratch_api_test.h \
               test/speed_test.h \
               test/digital_scratch_test.h \
               test/volume_test.h
}

OTHER_FILES += \
    AUTHORS \
    README \
    NEWS \
    INSTALL \
    COPYING \
    dist/ubuntu/generate_libdigitalscratch_deb.sh \
    dist/ubuntu/debian/rules \
    dist/ubuntu/debian/libdigitalscratch-dev.install \
    dist/ubuntu/debian/libdigitalscratch-dev.dirs \
    dist/ubuntu/debian/libdigitalscratch1.dirs \
    dist/ubuntu/debian/libdigitalscratch1.install \
    dist/ubuntu/debian/docs \
    dist/ubuntu/debian/copyright \
    dist/ubuntu/debian/control \
    dist/ubuntu/debian/compat \
    dist/ubuntu/debian/changelog \
    dist/debian/generate_libdigitalscratch_deb.sh \
    dist/debian/debian/changelog \
    dist/debian/debian/compat \
    dist/debian/debian/control \
    dist/debian/debian/copyright \
    dist/debian/debian/docs \
    dist/debian/debian/libdigitalscratch1.dirs \
    dist/debian/debian/libdigitalscratch1.install \
    dist/debian/debian/libdigitalscratch-dev.dirs \
    dist/debian/debian/libdigitalscratch-dev.install \
    dist/debian/debian/rules


############################
# Copy dll and .h for windows build
CONFIG(test) {
}
else {
    win32 {
        CONFIG(debug, debug|release) {
            OUT_PWD_WIN = $${OUT_PWD}/debug
        } else {
            OUT_PWD_WIN = $${OUT_PWD}/release
        }
        OUT_PWD_WIN ~= s,/,\\,g
        PWD_WIN = $${PWD}
        PWD_WIN ~= s,/,\\,g
        QMAKE_POST_LINK += $${QMAKE_COPY} $$quote($${OUT_PWD_WIN}\\digitalscratch*.lib) $$quote($${PWD_WIN}\\..\\digitalscratch\\win-external\\libdigitalscratch\\lib\\) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $${QMAKE_COPY} $$quote($${OUT_PWD_WIN}\\digitalscratch*.dll) $$quote($${PWD_WIN}\\..\\digitalscratch\\win-external\\libdigitalscratch\\lib\\) $$escape_expand(\\n\\t)
        QMAKE_POST_LINK += $${QMAKE_COPY} $$quote($${PWD_WIN}\\src\\include\\digital_scratch_api.h) $$quote($${PWD_WIN}\\..\\digitalscratch\\win-external\\libdigitalscratch\\include\\) $$escape_expand(\\n\\t)
    }
}

CONFIG(test) {
    win32 {
        DESTDIR_WIN = $${DESTDIR}
        CONFIG(debug, debug|release) {
            DESTDIR_WIN += debug
            DLLS = %QTDIR%/bin/Qt5Cored.dll \
                   %QTDIR%/bin/icuin*.dll \
                   %QTDIR%/bin/icuuc*.dll \
                   %QTDIR%/bin/icudt*.dll \
                   %QTDIR%/bin/Qt5Testd.dll
        } else {
            DESTDIR_WIN += release
            DLLS = %QTDIR%/bin/Qt5Core.dll \
                   %QTDIR%/bin/icuin*.dll \
                   %QTDIR%/bin/icuuc*.dll \
                   %QTDIR%/bin/icudt*.dll \
                   %QTDIR%/bin/Qt5Test.dll
        }
        DLLS ~= s,/,\\,g
        DESTDIR_WIN ~= s,/,\\,g
        for(FILE, DLLS){
            QMAKE_POST_LINK += $${QMAKE_COPY} $$quote($${FILE}) $$quote($${DESTDIR_WIN}) $$escape_expand(\\n\\t)
        }
    }
}
############################
