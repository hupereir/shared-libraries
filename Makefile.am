AUTOMAKE_OPTIONS = foreign
INCLUDES = \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui \
  -I$(aspell_includes) \
	-I$(top_srcdir)/base \
	-I$(top_srcdir)/base-qt
 
CXXFLAGS = @CXXFLAGS@

noinst_HEADERS = \
  AutoSpellConfig.h \
  SpellConfig.h \
  SpellDialog.h \
  SpellInterface.h \
  SpellParser.h \
  Word.h


noinst_LIBRARIES = libspellcheck.a
  
libspellcheck_a_SOURCES =  \
  AutoSpellConfig.cc \
  SpellConfig.cc \
  SpellDialog.cc \
  SpellInterface.cc \
  SpellParser.cc
	
nodist_libspellcheck_a_SOURCES = \
  moc_SpellDialog.cc
	
clean-local:
	 rm -f moc_*.cc

### rule to generate qt meta objects
moc_%.cc: %.h
	$(qt_moc) $^ -o $@
