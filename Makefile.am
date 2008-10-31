AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base-svg.pro svg.qrc background.svg background-translucent.svg

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
  Svg.h

noinst_LIBRARIES = libbase-svg.a
  
libbase_svg_a_SOURCES =  \
  SvgConfiguration.cpp \
  SvgEngine.cpp

nodist_libbase_svg_a_SOURCES =  \
  qrc_svg.cpp \
  moc_SvgConfiguration.cpp
	
clean-local:
	rm -f moc_*.cpp
	rm -f qrc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@

qrc_%.cpp: %.qrc
	$(qt_rcc) -name $* $^ -o $@
