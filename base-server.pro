TEMPLATE = lib
CONFIG = staticlib
CONFIG += qt
QT += xml network

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
  ApplicationId.h \
  ApplicationManager.h \
  BaseApplication.h \
  BaseCoreApplication.h \
  Client.h \
  ServerCommand.h

SOURCES = \
  ApplicationManager.cpp \
  BaseApplication.cpp \
  BaseCoreApplication.cpp \
	Client.cpp \
  ServerCommand.cpp
