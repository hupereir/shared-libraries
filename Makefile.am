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
  Command.h \
  Counter.h \
  CounterMap.h \
  CustomProcess.h \
  Debug.h \
  Exception.h \
  File.h \
  FileRecord.h \
  Key.h \
  Option.h \
  Options.h \
  Str.h \
  Singleton.h \
  TextFormat.h \
  TextFormatBlock.h \
  TextPosition.h \
  TimeStamp.h \
  Util.h

noinst_LIBRARIES = libbase.a

libbase_a_SOURCES = \
  ArgList.cpp \
  Command.cpp \
  Counter.cpp \
  CustomProcess.cpp \
 	Debug.cpp \
  File.cpp \
  FileRecord.cpp \
  Key.cpp \
  Options.cpp \
  Singleton.cpp \
  Str.cpp \
  TimeStamp.cpp \
  Util.cpp

nodist_libbase_a_SOURCES = \
  moc_CustomProcess.cpp

clean-local:
	rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
