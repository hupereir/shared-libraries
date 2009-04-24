TEMPLATE = lib
CONFIG = staticlib

CONFIG += qt
QT += xml svg

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
  SvgConfiguration.h \
  SvgEngine.h \
  SvgHelpText.h \
  SvgPlasmaInterface.h \
  SvgRenderer.h \
  SvgThread.h \
  Svg.h

SOURCES = \
  SvgConfiguration.cpp \
  SvgEngine.cpp \
  SvgPlasmaInterface.cpp \
  SvgRenderer.cpp \
  SvgThread.cpp
