TEMPLATE = lib
CONFIG = staticlib

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
AutoSpellConfig.h \
DictionaryMenu.h \
FilterMenu.h \
SpellCheckBlockData.h \
SpellConfig.h \
SpellDialog.h \
SpellInterface.h \
SpellParser.h \
SuggestionMenu.h \
Word.h

SOURCES = \
AutoSpellConfig.cc \
DictionaryMenu.cc \
FilterMenu.cc \
SpellCheckBlockData.cc \
SpellConfig.cc \
SpellDialog.cc \
SpellInterface.cc \
SpellParser.cc \
SuggestionMenu.cc

CONFIG += qt
QT += xml
