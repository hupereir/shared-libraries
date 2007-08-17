AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui

noinst_HEADERS = \
  ArgList.h \
  Counter.h \
  CustomProcess.h \
  Debug.h \
  Exception.h \
  File.h \
  FileList.h \
  FileRecord.h \
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
  CustomProcess.cc \
  Exception.cc \
  File.cc \
  FileList.cc \
  Key.cc \
  Options.cc \
  Str.cc \
  TimeStamp.cc \
  Util.cc \
  XmlUtil.cc
	
nodist_libbase_a_SOURCES = \
  moc_CustomProcess.cc

clean-local:
	rm -f moc_*.cc

### rule to generate qt meta objects
moc_%.cc: %.h
	$(qt_moc) $^ -o $@


