#!/bin/sh

QT_INCLUDE_PATH=/usr/include/x86_64-linux-gnu/qt5

# Run c++ static checker on the full src/ folder.
cppcheck --enable=all --max-configs=1 \
-I../digitalscratch/include \
-I../libdigitalscratch/src/include \
-I$QT_INCLUDE_PATH \
-I$QT_INCLUDE_PATH/QtWidgets \
-I$QT_INCLUDE_PATH/QtMultimedia \
-I$QT_INCLUDE_PATH/QtGui \
-I$QT_INCLUDE_PATH/QtSql \
-I$QT_INCLUDE_PATH/QtConcurrent \
-I$QT_INCLUDE_PATH/QtNetwork \
-I$QT_INCLUDE_PATH/QtTest \
-I$QT_INCLUDE_PATH/QtCore \
../digitalscratch/src/ \
../libdigitalscratch/src/
