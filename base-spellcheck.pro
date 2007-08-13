TEMPLATE = lib
CONFIG = staticlib

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
AutoSpellConfiguration.h \
DictionaryMenu.h \
FilterMenu.h \
SpellCheckConfiguration.h \
SpellDialog.h \
SpellInterface.h \
SpellParser.h \
SuggestionMenu.h \
Word.h

SOURCES = \
AutoSpellConfiguration.cc \
DictionaryMenu.cc \
FilterMenu.cc \
SpellCheckConfiguration.cc \
SpellDialog.cc \
SpellInterface.cc \
SpellParser.cc \
SuggestionMenu.cc

CONFIG += qt
QT += xml
