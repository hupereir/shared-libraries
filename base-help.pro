# $Id$

TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
DefaultHelpText.h \
HelpDialog.h \
HelpItem.h \
HelpManager.h \
HelpModel.h \
NewItemDialog.h

SOURCES = \
HelpDialog.cpp \
HelpItem.cpp \
HelpManager.cpp \
HelpModel.cpp \
NewItemDialog.cpp
