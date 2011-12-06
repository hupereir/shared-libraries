ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = base-svg.pro \
  baseSvg.qrc \
  background.svg \
  background-simple.svg \
  background-translucent.svg \
  background-white.svg


INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui \
  -I$(qt_includes)/QtSvg \
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

noinst_LIBRARIES = libbase-svg.a

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
AM_LDFLAGS = -L$(libdir) -L$(qt_libraries)

lib_LTLIBRARIES = libbase-svg.la
libbase_svg_la_LIBADD = -lQtSvg -lbase -lbase-qt
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
	$(qt_moc) $^ -o $@

qrc_%.cpp: %.qrc
	$(qt_rcc) -name $* $^ -o $@
