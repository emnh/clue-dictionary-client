
EXTRA_DIST = clue.i

CLEANFILES = *_wrap.cxx clue.py

lib_LTLIBRARIES = libPythonclue.la

nodist_python_PYTHON = clue.py

nodist_libPythonclue_la_SOURCES = clue_wrap.cxx
libPythonclue_la_LDFLAGS = -avoid-version -module
libPythonclue_la_LIBADD = $(top_srcdir)/libclue.a
libPythonclue_la_CPPFLAGS = $(SWIG_PYTHON_CPPFLAGS)

%.py: %_wrap.cxx

%_wrap.cxx: %.i $(top_srcdir)/%.h
	$(SWIG) $(SWIG_PYTHON_OPT) -I$(top_srcdir) -o $@ $<

install-exec-hook:
	$(mkinstalldirs) "$(DESTDIR)$(pythondir)"
	cd "$(DESTDIR)$(pythondir)" && rm -f _clue.so && $(LN_S) $(libdir)/libPythonclue.so _clue.so

uninstall-local:
	rm -f "$(DESTDIR)$(pythondir)/_clue.so"
