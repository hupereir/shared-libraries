TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
HelpDelegate.h \
HelpDialog.h \
HelpItem.h \
HelpManager.h \
HelpModel.h \
NewItemDialog.h

SOURCES = \
HelpDelegate.cpp \
HelpDialog.cpp \
HelpItem.cpp \
HelpManager.cpp \
HelpModel.cpp \
NewItemDialog.cpp
