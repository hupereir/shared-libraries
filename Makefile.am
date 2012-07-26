# $Id$

ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = ${QT_CFLAGS}
AM_LDFLAGS = ${QT_LIBS}

EXTRA_DIST = base-svg.pro \
baseSvg.qrc \
background.svg \
background-simple.svg \
background-translucent.svg \
background-white.svg

INCLUDES = \
-I$(top_srcdir)/base-qt \
-I$(top_srcdir)/base

noinst_HEADERS = \
SvgConfiguration.h \
SvgEngine.h \
SvgHelpText.h \
SvgPlasmaInterface.h \
SvgRenderer.h \
SvgThread.h \
SvgId.h \
Svg.h

sources =  \
SvgConfiguration.cpp \
SvgEngine.cpp \
SvgPlasmaInterface.cpp \
SvgRenderer.cpp \
SvgThread.cpp

nodist_sources =  \
moc_SvgEngine.cpp \
moc_SvgPlasmaInterface.cpp \
qrc_baseSvg.cpp

################
# shared library
if ENABLE_SHARED
lib_LTLIBRARIES = libbase-svg.la
libbase_svg_la_LIBADD = -lbase -lbase-qt
libbase_svg_la_SOURCES =  $(sources)
nodist_libbase_svg_la_SOURCES = $(nodist_sources)
libbase_svg_la_CXXFLAGS = $(AM_CXXFLAGS)

# executable
BUILT_SOURCES = testexternals.c
noinst_PROGRAMS = testexternals
testexternals_LDADD = libbase-svg.la

testexternals.c:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

else

################
# static library
noinst_LIBRARIES = libbase-svg.a
libbase_svg_a_SOURCES = $(sources)
nodist_libbase_svg_a_SOURCES = $(nodist_sources)
libbase_svg_a_XXCFLAGS = $(AM_CXXFLAGS)

endif

# clean
clean-local:
	rm -f moc_*.cpp
	rm -f qrc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	moc $^ -o $@

qrc_%.cpp: %.qrc
	rcc -name $* $^ -o $@
