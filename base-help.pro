TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
 HelpDialog.h \
 HelpItem.h \
 HelpItemList.h \
 HelpManager.h

SOURCES = \
 HelpDialog.cpp \
 HelpItem.cpp \
 HelpManager.cpp
