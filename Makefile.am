ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = base-server.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtDBus  \
  -I$(qt_includes)/QtGui \
  -I$(qt_includes)/QtNetwork \
  -I$(qt_includes)/QtXml \
	-I$(top_srcdir)/base \
	-I$(top_srcdir)/base-qt

noinst_HEADERS = \
  ApplicationId.h \
  ApplicationManager.h \
  BaseApplication.h \
  BaseCoreApplication.h \
  Client.h \
  ServerCommand.h \
  ServerConfiguration.h \
  ServerXmlDef.h

sources = \
  ApplicationId.cpp \
  ApplicationManager.cpp \
  BaseApplication.cpp \
  BaseCoreApplication.cpp \
  Client.cpp \
  ServerCommand.cpp \
  ServerConfiguration.cpp

nodist_sources = \
  moc_ApplicationManager.cpp \
  moc_BaseApplication.cpp \
  moc_BaseCoreApplication.cpp \
  moc_Client.cpp

# shared library
if ENABLE_SHARED
AM_LDFLAGS = -L$(libdir) -L$(qt_libraries)

lib_LTLIBRARIES = libbase-server.la
libbase_server_la_LIBADD = -lbase -lbase-qt -l$(qt_lib) -lQtDBus

libbase_server_la_SOURCES =  $(sources)
nodist_libbase_server_la_SOURCES = $(nodist_sources)
libbase_server_la_CXXFLAGS = $(AM_CXXFLAGS)

# executable
BUILT_SOURCES = testexternals.c
noinst_PROGRAMS = testexternals
testexternals_LDADD = libbase-server.la

testexternals.c:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

else

# static library
noinst_LIBRARIES = libbase-server.a
libbase_server_a_SOURCES = $(sources)
nodist_libbase_server_a_SOURCES = $(nodist_sources)
libbase_server_a_XXCFLAGS = $(AM_CXXFLAGS)

endif

# clean
clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
