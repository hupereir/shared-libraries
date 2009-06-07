TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
BackgroundPixmap.h \
CompositeEngine.h \
Effects.h \
TransparencyConfiguration.h \
TransparencyHelpText.h \
TransparentWidget.h

SOURCES = \
BackgroundPixmap.cpp \
CompositeEngine.cpp \
TransparencyConfiguration.cpp \
TransparentWidget.cpp
