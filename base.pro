TEMPLATE = lib
CONFIG = staticlib qt
QT += network

HEADERS = \
  Command.h \
  CommandLineArguments.h \
  CommandLineParser.h \
  Counter.h \
  CounterMap.h \
  CustomProcess.h \
  Debug.h \
  File.h \
  FileRecord.h \
  Key.h \
  Option.h \
  Options.h \
  QOrderedSet.h \
  Str.h \
  Singleton.h \
  TextFormat.h \
  TextFormatBlock.h \
  TimeStamp.h \
  Util.h

SOURCES = \
  Command.cpp \
  CommandLineArguments.cpp \
  CommandLineParser.cpp \
  Counter.cpp \
  CounterMap.cpp \
  CustomProcess.cpp \
  Debug.cpp \
  File.cpp \
  FileRecord.cpp \
  Key.cpp \
  Option.cpp \
  Options.cpp \
  Singleton.cpp \
  Str.cpp \
  TimeStamp.cpp \
  Util.cpp
