#
# Digital-scratch-api-tests autoconf configuration script
#
# Julien Rosener <julien.rosener@digital-scratch.org>
#


# Process this file with autoconf to produce a configure script.


# Init autoconf, init version and package name, init automake, add reference on config.h
AC_PREREQ(2.59)
AC_INIT(digitalscratchtest, 1.0.0, dscratch-devel@gna.org)
AM_INIT_AUTOMAKE(digitalscratchtest, 1.0.0)
AM_CONFIG_HEADER(config.h)
AC_PREFIX_DEFAULT(/usr)


# Check compiler
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_CC


# Use libtool
AC_PROG_LIBTOOL


# Enable debug suppport (compile with GCC debug flags)
isdebug=${isdebug="false"}
AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug], [Compile with gcc debug flags])],
[
    if test "x${enableval}" != "xno"; then
        CXXFLAGS="-g -Wall"
        isdebug="true"
    fi
], [CXXFLAGS="-O3 -Wall"])

# Enable profiling suppport (compile with -pg and debug flags)
staticlink=${staticlink="false"}
AC_ARG_ENABLE(profiler, [AC_HELP_STRING([--enable-profiler], [Compile with gcc profiling flags])],
[
    if test "x${enableval}" != "xno"; then
        CXXFLAGS="-g -Wall -pg"
        isdebug="true"
        staticlink="true"
    fi
], [])

# Enable Valgrind check.
AC_ARG_ENABLE(valgrind, [AC_HELP_STRING([--enable-valgrind], [Compile to use Valgrind later])],
[
    if test "x${enableval}" != "xno"; then
        CXXFLAGS="-g -Wall"
        isdebug="true"
        #staticlink="true"
    fi
], [])

# Enable static link
AC_ARG_ENABLE(staticlink, [AC_HELP_STRING([--enable-staticlink], [Link with digital-scratch static lib])],
[
    if test "x${enableval}" != "xno"; then
        staticlink="true"
    fi
], [])

if test "x${staticlink}" = "xfalse"; then
    # Check if Digital-scratch is present.
    AC_SEARCH_LIBS(dscratch_create_turntable, digitalscratch, [], [AC_MSG_ERROR([libdigitalscratch missing ?], 1)])
else
    # Link digital-scratch statically.
    LIBS+=" /usr/lib/libdigitalscratch.a"
fi

# Include digital-scratch header files.
CPPFLAGS+=" -I../../src/include"

# Add libboost support.
LIBS+=" -lboost_unit_test_framework"
CPPFLAGS+=" -DBOOST_TEST_DYN_LINK"

# Display a resume of user options
echo " "
echo "-- Resume -----------------------"

if test "x${isdebug}" = "xtrue"; then
    echo "Compilation type : debug"
else
    echo "Compilation type : release"
fi

echo "CXXFLAGS : $CXXFLAGS"
echo "CPPFLAGS : $CPPFLAGS"
echo "LDFLAGS : $LDFLAGS"
echo "LIBS : $LIBS"

echo "---------------------------------"


# Generate Makefiles
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
