TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
TransparencyHelpText.h \
BackgroundPixmap.h \
CompositeEngine.h \
TransparencyConfiguration.h \
TransparentWidget.h

SOURCES = \
BackgroundPixmap.cpp \
CompositeEngine.cpp \
TransparencyConfiguration.cpp \
TransparentWidget.cpp
