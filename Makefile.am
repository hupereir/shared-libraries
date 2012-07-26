# $Id$

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = ${QT_CFLAGS}
AM_LDFLAGS = ${QT_LIBS}

EXTRA_DIST = base-help.pro

INCLUDES = \
-I$(top_srcdir)/base \
-I$(top_srcdir)/base-qt

noinst_HEADERS = \
DefaultHelpText.h \
HelpDialog.h \
HelpItem.h \
HelpManager.h \
HelpModel.h \
NewItemDialog.h

sources = \
HelpDialog.cpp \
HelpItem.cpp \
HelpManager.cpp \
HelpModel.cpp \
NewItemDialog.cpp

nodist_sources = \
moc_HelpDialog.cpp \
moc_HelpManager.cpp \
moc_HelpModel.cpp

################
# shared library
if ENABLE_SHARED
lib_LTLIBRARIES = libbase-help.la
libbase_help_la_LIBADD = -lbase -lbase-qt
libbase_help_la_SOURCES =  $(sources)
nodist_libbase_help_la_SOURCES = $(nodist_sources)
libbase_help_la_CXXFLAGS = $(AM_CXXFLAGS)

# executable
BUILT_SOURCES = testexternals.c
noinst_PROGRAMS = testexternals
testexternals_LDADD = libbase-help.la

testexternals.c:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

else

################
# static library
noinst_LIBRARIES = libbase-help.a
libbase_help_a_SOURCES = $(sources)
nodist_libbase_help_a_SOURCES = $(nodist_sources)
libbase_help_a_XXCFLAGS = $(AM_CXXFLAGS)

endif

# clean
clean-local:
	rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	moc $^ -o $@
