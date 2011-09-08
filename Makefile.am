ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = base-server.pro

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
  ApplicationId.h \
  ApplicationManager.h \
  BaseApplication.h \
  BaseCoreApplication.h \
  Client.h \
  ServerCommand.h \
  ServerConfiguration.h \
  ServerXmlDef.h

noinst_LIBRARIES = libbase-server.a

libbase_server_a_SOURCES = \
  ApplicationId.cpp \
  ApplicationManager.cpp \
  BaseApplication.cpp \
  BaseCoreApplication.cpp \
  Client.cpp \
  ServerCommand.cpp \
  ServerConfiguration.cpp

nodist_libbase_server_a_SOURCES = \
  moc_ApplicationManager.cpp \
  moc_BaseApplication.cpp \
	moc_BaseCoreApplication.cpp \
  moc_Client.cpp

clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
