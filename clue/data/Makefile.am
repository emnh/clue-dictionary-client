
CLUE = $(top_srcdir)/clue
DATADIR = $(datadir)/clue
DESTDATADIR = $(DESTDIR)$(DATADIR)

all: process

zips = $(wildcard $(srcdir)/*.zip) $(wildcard $(srcdir)/*.ZIP)

$(CLUE):
	$(MAKE) -C $(top_srcdir) clue

decrypted/%.DAT: cluedata/%.DAT
	$(CLUE) --install -f $< > $@

process: $(CLUE)
	test -d decrypted || mkdir decrypted
	@$(foreach zip, $(zips), $(UNZIP) -n $(zip) cluedata/\*.DAT | sed 1d;)
	@$(MAKE) --no-print-directory decrypt
	
decrypt: $(patsubst cluedata/%.DAT, $(srcdir)/decrypted/%.DAT, \
	 $(wildcard cluedata/*.DAT))

install: all
	$(mkinstalldirs) $(DESTDATADIR)
	find $(srcdir)/decrypted -iname \*.DAT \
		-exec $(INSTALL_DATA) {} $(DESTDATADIR) \;

clean:
	rm -rf cluedata decrypted
