TEMPLATE = lib
CONFIG = staticlib

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
  ApplicationId.h \
  ApplicationManager.h \
  BaseApplication.h \
  Client.h \
  Server.h \
  ServerCommand.h

SOURCES = \
  ApplicationManager.cpp \
  BaseApplication.cpp \
  Client.cpp \
  Server.cpp \
  ServerCommand.cpp

CONFIG += qt
QT += xml network
