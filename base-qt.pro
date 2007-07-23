TEMPLATE = lib
CONFIG = staticlib

INCLUDEPATH += ../base
DEPENDPATH += . ../base

HEADERS = \
  BrowsedLineEdit.h \
  ClockLabel.h \
  ColorDisplay.h \
  CounterDialog.h \
  CounterList.h \
  CustomDialog.h \
  CustomFileDialog.h \
  CustomLineEdit.h \
  CustomListBox.h \
  CustomListView.h \
  CustomPixmap.h \
  DebugMenu.h \
  ErrorHandler.h \
  IconBrowsedButton.h \
  QtUtil.h \
  StateFrame.h \
  XmlError.h \
  XmlOption.h \
  XmlOptions.h \
  XmlTimeStamp.h 

SOURCES = \
  BrowsedLineEdit.cc \
  ClockLabel.cc \
  ColorDisplay.cc \
  CounterDialog.cc \
  CounterList.cc \
  CustomDialog.cc \
  CustomFileDialog.cc \
  CustomLineEdit.cc \
  CustomListBox.cc \
  CustomListView.cc \
  CustomPixmap.cc \
  DebugMenu.cc \
  ErrorHandler.cc \
  IconBrowsedButton.cc \
  QtUtil.cc \
  StateFrame.cc \
  XmlOption.cc \
  XmlOptions.cc \
  XmlTimeStamp.cc     

CONFIG += qt
QT += xml
