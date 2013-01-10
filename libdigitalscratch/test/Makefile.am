#
# Digital-scratch automake configuration script
#
# Julien Rosener, 2007.
#

## Process this file with automake to produce makefile.in

SUBDIRS = src

check: test

check-quiet: test-quiet

check-log: test-log

check-random: test-random

check-leak:
	valgrind --leak-check=yes src/digital-scratch-test 2&> leak.txt

check-call:
	valgrind --tool=callgrind src/digital-scratch-test
	echo "!!! Now call Kcachegrind and open the profil file (callgrind.out.*)."

# Basic display
test:
	src/digital-scratch-test --report_level=detailed --show_progress=yes

# Do not display trace_errors()
test-quiet:
	src/digital-scratch-test --report_level=detailed --show_progress=yes 2>/dev/null

# Display a lot of Boot.Test informations
test-log:
	src/digital-scratch-test --report_level=detailed --show_progress=yes --log_level=all

# Basic display but run tests randomly
test-random:
	src/digital-scratch-test --report_level=detailed --show_progress=yes --random=1

# generate gprof informations
gprof: check-quiet
	gprof src/digital-scratch-test gmon.out
