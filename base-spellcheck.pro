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
SpellItemDialog.h \
SpellItemModel.h \
SpellParser.h \
SuggestionMenu.h \
	Word.h

SOURCES = \
AutoSpellConfiguration.cpp \
DictionaryMenu.cpp \
FilterMenu.cpp \
SpellCheckConfiguration.cpp \
SpellDialog.cpp \
SpellInterface.cpp \
SpellItemDialog.cpp \
SpellItemModel.cpp \
SpellParser.cpp \
SuggestionMenu.cpp

CONFIG += qt
QT += xml
