TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
BackgroundPixmap.h \
TransparentWidget.h

SOURCES = \
BackgroundPixmap.cpp \
TransparentWidget.cpp
