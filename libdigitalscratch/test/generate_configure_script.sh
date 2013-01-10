#! /bin/bash

#
# Generate a configure script for digital-scratch-api-tests
#

echo "Generating aclocal.m4..."
aclocal

echo "Calling autoconf (parse configure.ac)..."
autoconf

echo "Calling autoheader..."
autoheader

echo "Calling automake (parse Makefile.am)..."
automake -a -c

echo "Calling libtoolize..."
libtoolize --force

echo "--Done (configure script should be generated)"
