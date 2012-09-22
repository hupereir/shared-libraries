# $Id$

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = ${QT_CFLAGS}
AM_LDFLAGS = ${QT_LIBS}

EXTRA_DIST = base-spellcheck.pro

INCLUDES =  \
-I$(top_srcdir)/base-qt \
-I$(top_srcdir)/base

noinst_HEADERS = \
AutoSpellConfiguration.h \
DictionaryMenu.h \
DictionarySelectionButton.h \
FilterMenu.h \
FilterSelectionButton.h \
SpellCheckConfiguration.h \
SpellDialog.h \
SpellInterface.h \
SpellItemDialog.h \
SpellItemModel.h \
SpellParser.h \
SuggestionMenu.h \
Word.h

sources =  \
AutoSpellConfiguration.cpp \
DictionaryMenu.cpp \
DictionarySelectionButton.cpp \
FilterMenu.cpp \
FilterSelectionButton.cpp \
SpellCheckConfiguration.cpp \
SpellDialog.cpp \
SpellInterface.cpp \
SpellItemDialog.cpp \
SpellItemModel.cpp \
SpellParser.cpp \
SuggestionMenu.cpp

nodist_sources = \
moc_DictionaryMenu.cpp \
moc_DictionarySelectionButton.cpp \
moc_FilterMenu.cpp \
moc_FilterSelectionButton.cpp \
moc_SpellCheckConfiguration.cpp \
moc_SpellDialog.cpp \
moc_SpellItemDialog.cpp \
moc_SuggestionMenu.cpp

# executable
BUILT_SOURCES = testexternals.c
noinst_PROGRAMS = testexternals

testexternals.c:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################
# shared library
if ENABLE_SHARED
lib_LTLIBRARIES = libbase-spellcheck.la
libbase_spellcheck_la_LIBADD = -lbase -lbase-qt -laspell
libbase_spellcheck_la_SOURCES =  $(sources)
nodist_libbase_spellcheck_la_SOURCES = $(nodist_sources)
libbase_spellcheck_la_CXXFLAGS = $(AM_CXXFLAGS)

testexternals_LDADD = libbase-spellcheck.la

else

################
# static library
noinst_LIBRARIES = libbase-spellcheck.a
libbase_spellcheck_a_SOURCES = $(sources)
nodist_libbase_spellcheck_a_SOURCES = $(nodist_sources)
libbase_spellcheck_a_XXCFLAGS = $(AM_CXXFLAGS)

testexternals_LDADD = \
  libbase-spellcheck.a \
  ../base-qt/libbase-qt.a \
  ../base/libbase.a \
  -laspell

endif

# clean
clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	moc $^ -o $@
