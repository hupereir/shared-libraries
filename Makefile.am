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
  ArgList.cpp \
  Counter.cpp \
  CustomProcess.cpp \
	 Debug.cpp \
  Exception.cpp \
  File.cpp \
  FileList.cpp \
  Key.cpp \
  Options.cpp \
  Str.cpp \
  TimeStamp.cpp \
  Util.cpp \
  XmlUtil.cpp
	
nodist_libbase_a_SOURCES = \
  moc_CustomProcess.cpp

clean-local:
	rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@


