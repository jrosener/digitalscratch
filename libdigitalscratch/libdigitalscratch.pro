VERSION = 1.6.2
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

############################
# Windows binary metadata.
win32:QMAKE_TARGET_PRODUCT = "digitalscratch.dll"
win32:QMAKE_TARGET_COPYRIGHT = "GPL V3"
############################

SOURCES += \ 
    src/serato_vinyl.cpp \
    src/final_scratch_vinyl.cpp \
    src/digital_scratch_api.cpp \
    src/digital_scratch.cpp \
    src/controller.cpp \
    src/coded_vinyl.cpp \
    src/mixvibes_vinyl.cpp \
    src/log.cpp \
    src/iir_filter.cpp \
    src/inst_freq_extractor.cpp

HEADERS += \ 
    src/include/serato_vinyl.h \
    src/include/final_scratch_vinyl.h \
    src/include/dscratch_parameters.h \
    src/include/digital_scratch_api.h \
    src/include/digital_scratch.h \
    src/include/controller.h \
    src/include/coded_vinyl.h \
    src/include/mixvibes_vinyl.h \
    src/include/log.h \
    src/include/iir_filter.h \
    src/include/inst_freq_extrator.h

CONFIG(test) {
    INCLUDEPATH += test

    SOURCES += test/main_test.cpp \
               test/test_utils.cpp \
               test/digital_scratch_api_test.cpp \
               test/digital_scratch_test.cpp

    HEADERS += test/test_utils.h \
               test/digital_scratch_api_test.h \
               test/digital_scratch_test.h
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
