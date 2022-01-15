AUTOMAKE_OPTIONS = 1.4

SUBDIRS = libraries ofs-gui src
MYEXECPDIR =${sbindir}

ACLOCAL_AMFLAGS = -I m4

README: README.md
	cat $< > $@

AUTHORS: .FORCE
	git shortlog --summary --numbered --email | cut -f2 > $@

.FORCE:

.PHONY: .FORCE
