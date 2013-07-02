#-------------------------------------------------
#
# Project created by QtCreator 2010-11-08T23:36:00
#
#-------------------------------------------------


CONFIG(test) {
    QT       += testlib
    TARGET    = libdigitalscratch-test
    CONFIG   += console
    CONFIG   -= app_bundle
    VERSION   = 1.0.0
}
else {
    QT -= core gui

    TARGET = digitalscratch
    TEMPLATE = lib

    DEFINES += DIGITALSCRATCH_LIBRARY
    VERSION  = 1.3.0

    target.path = /usr/lib

    include.path = /usr/include
    include.files = src/include/digital_scratch_api.h

    INSTALLS += target include
}
DEFINES += VERSION=$${VERSION}

SOURCES += \ 
    src/volume.cpp \
    src/utils.cpp \
    src/speed.cpp \
    src/serato_vinyl.cpp \
    src/position.cpp \
    src/playing_parameter.cpp \
    src/final_scratch_vinyl.cpp \
    src/digital_scratch_api.cpp \
    src/digital_scratch.cpp \
    src/controller.cpp \
    src/coded_vinyl.cpp \
    src/mixvibes_vinyl.cpp

HEADERS += \ 
    src/include/volume.h \
    src/include/utils.h \
    src/include/timecode.h \
    src/include/speed.h \
    src/include/serato_vinyl.h \
    src/include/position.h \
    src/include/playing_parameter.h \
    src/include/final_scratch_vinyl.h \
    src/include/dscratch_parameters.h \
    src/include/digital_scratch_api.h \
    src/include/digital_scratch.h \
    src/include/controller.h \
    src/include/coded_vinyl.h \
    src/include/mixvibes_vinyl.h

CONFIG(test) {
    INCLUDEPATH += test src/include

    SOURCES += test/main_test.cpp \
               test/test_utils.cpp \
               test/controller_test.cpp \
               test/digital_scratch_api_test.cpp
               #test/speed_test.cpp \
               #test/position_test.cpp \
               #test/digital_scratch_test_suite.cpp \
               #test/digital_scratch_test.cpp \
               #test/volume_test.cpp \               
               #test/coded_vinyl_test.cpp

    HEADERS += test/test_utils.h \
               test/controller_test.h \
               test/digital_scratch_api_test.h
               #test/test_utils.h
}

OTHER_FILES += \
    AUTHORS \
    README \
    NEWS \
    INSTALL \
    COPYING \
    debian/rules \
    debian/libdigitalscratch-dev.install \
    debian/libdigitalscratch-dev.dirs \
    debian/libdigitalscratch1.install \
    debian/generate_libdigitalscratch_deb.sh \
    debian/docs \
    debian/copyright \
    debian/control \
    debian/compat \
    debian/changelog

############################
# Copy dll and .h for windows build
CONFIG(!test) {
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
                   %QTDIR%/bin/icuin49.dll \
                   %QTDIR%/bin/icuuc49.dll \
                   %QTDIR%/bin/icudt49.dll \
                   %QTDIR%/bin/Qt5Testd.dll
        } else {
            DESTDIR_WIN += release
            DLLS = %QTDIR%/bin/Qt5Core.dll \
                   %QTDIR%/bin/icuin49.dll \
                   %QTDIR%/bin/icuuc49.dll \
                   %QTDIR%/bin/icudt49.dll \
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
