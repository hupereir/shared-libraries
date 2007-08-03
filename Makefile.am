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
  Client.h \
  Server.h \
  ServerCommand.h

noinst_LIBRARIES = libbase-server.a

libbase_server_a_SOURCES = \
  ApplicationManager.cc \
  Client.cc \
  Server.cc \
  ServerCommand.cc

nodist_libbase_server_a_SOURCES = \
  moc_ApplicationManager.cc \
  moc_Client.cc \
  moc_Server.cc

clean-local:
	 rm -f moc_*.cc

### rule to generate qt meta objects
moc_%.cc: %.h
	$(qt_moc) $^ -o $@
