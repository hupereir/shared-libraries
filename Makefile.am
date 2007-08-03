AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base-server.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui \
	-I$(top_srcdir)/base-qt \
	-I$(top_srcdir)/base

noinst_HEADERS = \
  AutoSpellConfig.h \
  DictionaryMenu.h \
  FilterMenu.h \
  SpellConfig.h \
  SpellDialog.h \
  SpellInterface.h \
  SpellParser.h \
  SuggestionMenu.h \
  Word.h

noinst_LIBRARIES = libbase-spellcheck.a
  
libbase_spellcheck_a_SOURCES =  \
  AutoSpellConfig.cc \
  DictionaryMenu.cc \
  FilterMenu.cc \
  SpellCheckBlockData.cc \
  SpellConfig.cc \
  SpellDialog.cc \
  SpellInterface.cc \
  SpellParser.cc \
  SuggestionMenu.cc
	
nodist_libbase_spellcheck_a_SOURCES =  \
  moc_DictionaryMenu.cc \
  moc_FilterMenu.cc \
  moc_SpellDialog.cc \
  moc_SuggestionMenu.cc
	
clean-local:
	 rm -f moc_*.cc

### rule to generate qt meta objects
moc_%.cc: %.h
	$(qt_moc) $^ -o $@
