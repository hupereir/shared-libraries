TEMPLATE = lib
CONFIG = staticlib
CONFIG += qt
QT += xml network

unix {
  QT += dbus
}

INCLUDEPATH += ../base ../base-qt
DEPENDPATH += . ../base ../base-qt

HEADERS = \
  ApplicationId.h \
  ApplicationManager.h \
  BaseApplication.h \
  BaseCoreApplication.h \
  Client.h \
  ServerCommand.h \
  ServerConfiguration.h \
  ServerXmlDef.h

SOURCES = \
  ApplicationId.cpp \
  ApplicationManager.cpp \
  BaseApplication.cpp \
  BaseCoreApplication.cpp \
  Client.cpp \
  ServerCommand.cpp \
  ServerConfiguration.cpp
