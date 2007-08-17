AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base-spellcheck.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui \
	-I$(top_srcdir)/base-qt \
	-I$(top_srcdir)/base

noinst_HEADERS = \
  AutoSpellConfiguration.h \
  DictionaryMenu.h \
  FilterMenu.h \
  SpellCheckConfiguration.h \
  SpellDialog.h \
  SpellInterface.h \
  SpellParser.h \
  SuggestionMenu.h \
  Word.h

noinst_LIBRARIES = libbase-spellcheck.a
  
libbase_spellcheck_a_SOURCES =  \
  AutoSpellConfiguration.cpp \
  DictionaryMenu.cpp \
  FilterMenu.cpp \
  SpellCheckConfiguration.cpp \
  SpellDialog.cpp \
  SpellInterface.cpp \
  SpellParser.cpp \
  SuggestionMenu.cpp
	
nodist_libbase_spellcheck_a_SOURCES =  \
  moc_DictionaryMenu.cpp \
  moc_FilterMenu.cpp \
  moc_SpellDialog.cpp \
  moc_SuggestionMenu.cpp
	
clean-local:
	 rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
