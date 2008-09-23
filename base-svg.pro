TEMPLATE = lib
CONFIG = staticlib

CONFIG += qt
QT += xml svg

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
  SvgConfiguration.h \
  SvgEngine.h \
  Svg.h

SOURCES = \
  SvgConfiguration.cpp \
  SvgEngine.cpp
