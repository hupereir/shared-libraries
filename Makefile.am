AUTOMAKE_OPTIONS = foreign

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui

EXTRA_DIST = base.pro

noinst_HEADERS = \
  ArgList.h \
  Counter.h \
  Debug.h \
  Exception.h \
  Key.h \
  Option.h \
  Options.h \
  Str.h \
  TextFormat.h \
  TextFormatBlock.h \
  TextPosition.h \
  TimeStamp.h \
  Util.h \
  XmlUtil.h

noinst_LIBRARIES = libbase.a

libbase_a_SOURCES = \
  ArgList.cc \
  Counter.cc \
  Exception.cc \
  Key.cc \
  Options.cc \
  Str.cc \
  TimeStamp.cc \
  Util.cc \
  XmlUtil.cc
	
