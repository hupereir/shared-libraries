# $Id$

TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
BackgroundPixmap.h \
CompositeEngine.h \
Effects.h \
ShadowLabel.h \
TransparencyConfiguration.h \
TransparencyHelpText.h \
TransparentWidget.h

SOURCES = \
BackgroundPixmap.cpp \
CompositeEngine.cpp \
ShadowLabel.cpp \
TransparencyConfiguration.cpp \
TransparentWidget.cpp
