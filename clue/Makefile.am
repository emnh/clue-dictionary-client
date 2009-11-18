
AUTOMAKE_OPTIONS = foreign

if WANT_PYTHON
MAYBE_BINDINGS = bindings
endif

SUBDIRS = . data $(MAYBE_BINDINGS)

bin_PROGRAMS = clue

noinst_LIBRARIES = libclue.a

libclue_a_SOURCES = \
cluefile.cc \
cluerecord.cc \
common.c \
install.c \
libclue.c \
mapping.c \
clue.h \
accents.h

clue_SOURCES = clue.cc
clue_LDADD = libclue.a
clue_CPPFLAGS = -Wall
clue_CPPFLAGS += -DDEFAULT_DATA_DIR="\"$(pkgdatadir)\""

man_MANS = clue.1

EXTRA_DIST = clue.1.in

edit = sed \
   -e 's,@PACKAGE_VERSION\@,$(PACKAGE_VERSION),g' \
   -e 's,@PACKAGE_BUGREPORT\@,$(PACKAGE_BUGREPORT),g' \
   -e 's,@datadir\@,$(pkgdatadir),g' \
   -e 's,@prefix\@,$(prefix),g'

clue.1: Makefile clue.1.in
	$(edit) clue.1.in > clue.1
