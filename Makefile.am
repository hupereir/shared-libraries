# $Id$

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign

if WITH_ASPELL
SPELLCHECK_SOURCE_DIR=base-spellcheck
else
SPELLCHECK_SOURCE_DIR=
endif

SUBDIRS =  \
  base \
  base-qt \
  base-help \
  base-server \
  base-svg \
  base-transparency \
  base-pixmaps \
	$(SPELLCHECK_SOURCE_DIR)

EXTRA_DIST = \
 INSTALL \
 COPYING\
 autogen.sh
