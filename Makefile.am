AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base-help.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui \
  -I$(top_srcdir)/base \
  -I$(top_srcdir)/base-qt 

noinst_HEADERS = \
	DefaultHelpText.h \
  HelpDialog.h \
  HelpItem.h \
  HelpManager.h \
  HelpModel.h \
  NewItemDialog.h 

noinst_LIBRARIES = libbase-help.a

libbase_help_a_SOURCES = \
  HelpDialog.cpp \
  HelpItem.cpp \
  HelpManager.cpp \
  HelpModel.cpp \
  NewItemDialog.cpp 

nodist_libbase_help_a_SOURCES = \
  moc_HelpDialog.cpp \
  moc_HelpManager.cpp \
  moc_HelpModel.cpp 

clean-local:
	rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
