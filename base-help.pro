TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
HelpDialog.h \
HelpItem.h \
HelpItemList.h \
HelpManager.h \
NewItemDialog.h

SOURCES = \
HelpDialog.cpp \
HelpItem.cpp \
HelpItemList.cpp \
HelpManager.cpp \
NewItemDialog.cpp
