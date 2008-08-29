AUTOMAKE_OPTIONS = foreign
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
  Client.h \
  Server.h \
  ServerCommand.h

noinst_LIBRARIES = libbase-server.a

libbase_server_a_SOURCES = \
  ApplicationManager.cpp \
  BaseApplication.cpp \
  Client.cpp \
  Server.cpp \
  ServerCommand.cpp

nodist_libbase_server_a_SOURCES = \
  moc_ApplicationManager.cpp \
  moc_BaseApplication.cpp \
  moc_Client.cpp \
  moc_Server.cpp

clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
