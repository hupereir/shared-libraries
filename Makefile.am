AUTOMAKE_OPTIONS = foreign
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
  TransparencyConfiguration.h \
  TransparentWidget.h

noinst_LIBRARIES = libbase-transparency.a

libbase_transparency_a_SOURCES = \
  BackgroundPixmap.cpp \
  TransparencyConfiguration.cpp \
  TransparentWidget.cpp

nodist_libbase_transparency_a_SOURCES = \
  moc_BackgroundPixmap.cpp \
  moc_TransparentWidget.cpp
  
clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
