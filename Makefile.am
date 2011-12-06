ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = base-transparency.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtGui \
  -I$(qt_includes)/QtNetwork \
  -I$(qt_includes)/QtXml \
	-I$(top_srcdir)/base \
	-I$(top_srcdir)/base-qt

noinst_HEADERS = \
  BackgroundPixmap.h \
  CompositeEngine.h \
  Effects.h \
  ShadowLabel.h \
  TransparencyConfiguration.h \
  TransparencyHelpText.h \
  TransparentWidget.h

noinst_LIBRARIES = libbase-transparency.a

sources = \
  BackgroundPixmap.cpp \
  CompositeEngine.cpp \
  ShadowLabel.cpp \
  TransparencyConfiguration.cpp \
  TransparentWidget.cpp

nodist_sources = \
  moc_BackgroundPixmap.cpp \
  moc_TransparentWidget.cpp

################
# shared library
if ENABLE_SHARED
AM_LDFLAGS = -L$(libdir) -L$(qt_libraries)

lib_LTLIBRARIES = libbase-transparency.la
libbase_transparency_la_LIBADD = -lbase -lbase-qt
libbase_transparency_la_SOURCES =  $(sources)
nodist_libbase_transparency_la_SOURCES = $(nodist_sources)
libbase_transparency_la_CXXFLAGS = $(AM_CXXFLAGS)

# executable
BUILT_SOURCES = testexternals.c
noinst_PROGRAMS = testexternals
testexternals_LDADD = libbase-transparency.la

testexternals.c:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

else

################
# static library
noinst_LIBRARIES = libbase-transparency.a
libbase_transparency_a_SOURCES = $(sources)
nodist_libbase_transparency_a_SOURCES = $(nodist_sources)
libbase_transparency_a_XXCFLAGS = $(AM_CXXFLAGS)

endif

# clean
clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
