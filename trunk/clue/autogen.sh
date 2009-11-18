#!/bin/sh
if [ ! -s acinclude.m4 ]; then
    swigm=http://ac-archive.sourceforge.net/ac-archive/ac_pkg_swig.m4
    pythonm=http://ac-archive.sourceforge.net/ac-archive/ac_python_devel.m4
    echo Downloading swig autoconf macros.
    wget -q $swigm -O acinclude.m4
    echo Downloading python autoconf macros.
    wget -q $pythonm -O- >> acinclude.m4
fi
if ! fgrep -q 'synopsis AC_PROG_SWIG' acinclude.m4; then
    echo Error: missing swig autoconf macros.
    exit 1
fi
if ! fgrep -q 'synopsis AC_PYTHON_DEVEL' acinclude.m4; then
    echo Error: missing python autoconf macros.
    exit 1
fi
autoreconf -vi
