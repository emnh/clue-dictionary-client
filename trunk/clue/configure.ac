#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(clue, 0.9.6, emh@kvarteret.no)
AM_INIT_AUTOMAKE(1.9)
AC_CONFIG_SRCDIR([clue.cc])
AM_CONFIG_HEADER([config.h])

# Checks for configure options
AC_ARG_ENABLE(python, 
              [AC_HELP_STRING([--enable-python], [enable python bindings])])
AM_CONDITIONAL(WANT_PYTHON,[test "$enable_python" == "yes"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
#AC_PROG_RANLIB
AC_PROG_LIBTOOL

if test x"$enable_python" == xyes; then
    AM_PATH_PYTHON
    AC_PROG_SWIG([1.3.24])
    SWIG_ENABLE_CXX
    SWIG_PYTHON
    AC_PROG_LN_S
    if ! expr "$SWIG" : ".*/swig .*" > /dev/null; then
        AC_MSG_ERROR("cannot find swig. needed for generating python bindings.")
    fi
fi

AC_CHECK_PROG(UNZIP, unzip, unzip)

if test -z "$UNZIP"; then
    AC_MSG_ERROR("cannot find unzip. needed for data extraction.")
fi

# Checks for libraries.
AC_CHECK_LIB([popt], [poptGetContext], , 
             AC_MSG_ERROR([popt library is missing]))
AC_CHECK_HEADER([popt.h], , 
             AC_MSG_ERROR([popt header is missing. install libpopt-dev]))

AC_CHECK_LIB([readline], [readline], , 
             AC_MSG_ERROR([readline library is missing]))
AC_CHECK_HEADER([readline/readline.h], , 
             AC_MSG_ERROR([readline header is missing. install libreadline-dev]))


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([langinfo.h locale.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([getdelim nl_langinfo setlocale strcasecmp strdup strndup])

AC_SUBST(DATADIR, $datadir)
AC_CONFIG_FILES([Makefile data/Makefile bindings/Makefile])

AC_OUTPUT
