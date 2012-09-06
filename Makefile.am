# $Id$

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = ${QT_CFLAGS}
AM_LDFLAGS = ${QT_LIBS}

EXTRA_DIST = base.pro

noinst_HEADERS = \
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

sources = \
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

# executable
BUILT_SOURCES = testexternals.c
noinst_PROGRAMS = testexternals

testexternals.c:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################
# shared library
if ENABLE_SHARED
lib_LTLIBRARIES = libbase.la
libbase_la_SOURCES =  $(sources)
libbase_la_CXXFLAGS = $(AM_CXXFLAGS)

testexternals_LDADD = libbase.la

else

################
# static library
noinst_LIBRARIES = libbase.a
libbase_a_SOURCES = $(sources)
libbase_a_XXCFLAGS = $(AM_CXXFLAGS)

testexternals_LDADD = libbase.a

endif

# clean
clean-local:
	rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	moc $^ -o $@
