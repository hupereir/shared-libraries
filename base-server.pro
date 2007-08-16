TEMPLATE = lib
CONFIG = staticlib

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
  ApplicationId.h \
  ApplicationManager.h \
  Client.h \
  Server.h \
  ServerCommand.h

SOURCES = \
  ApplicationManager.cc \
  Client.cc \
  Server.cc \
  ServerCommand.cc

CONFIG += qt
QT += xml network
