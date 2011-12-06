ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign

if WITH_ASPELL
SPELLCHECK_SOURCE_DIR=base-spellcheck
else
SPELLCHECK_SOURCE_DIR=
endif

SUBDIRS =  \
  base \
  base-pixmaps \
  base-qt \
  base-help \
  base-server \
  base-svg \
  base-transparency \
	$(SPELLCHECK_SOURCE_DIR)
