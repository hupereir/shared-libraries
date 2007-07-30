TEMPLATE = lib
CONFIG = staticlib

INCLUDEPATH += ../base
DEPENDPATH += . ../base

HEADERS = \
BrowsedLineEdit.h \
ClockLabel.h \
ColorDisplay.h \
ColorMenu.h \
ConfigDialogBase.h \
CounterDialog.h \
CounterList.h \
CustomComboBox.h \
CustomDialog.h \
CustomFileDialog.h \
CustomGridLayout.h \
CustomLineEdit.h \
CustomListBox.h \
CustomListView.h \
CustomMainWindow.h \
CustomPixmap.h \
CustomProcess.h \
CustomSlider.h \
CustomTabWidget.h \
CustomTextEdit.h \
CustomToolBar.h \
CustomToolButton.h \
DebugMenu.h \
DockPanel.h \
ErrorHandler.h \
FindDialog.h \
FontEdit.h \
HelpDialog.h \
HelpItem.h \
HelpItemList.h \
HelpManager.h \
IconBrowsedButton.h \
IconEngine.h \
ListViewConfig.h \
MultipleClickCounter.h \
OpenPreviousMenu.h \
OptionBrowsedLineEdit.h \
OptionCheckBox.h \
OptionColorDisplay.h \
OptionComboBox.h \
OptionFontEdit.h \
OptionFontInfo.h \
OptionIconBrowsedButton.h \
OptionLineEdit.h \
OptionListBox.h \
OptionSlider.h \
OptionSpinBox.h \
OptionWidget.h \
QtUtil.h \
RemoveLineBuffer.h \
ReplaceDialog.h \
SelectLineDialog.h \
StatusBar.h \
TextBlockData.h \
TextSelection.h \
TextSeparator.h \
ValidFileThread.h \
XmlError.h \
XmlFileList.h \
XmlFileRecord.h \
XmlOption.h \
XmlOptions.h \
XmlTextFormatBlock.h \
XmlTimeStamp.h

SOURCES = \
BrowsedLineEdit.cc \
ClockLabel.cc \
ColorDisplay.cc \
ColorMenu.cc \
ConfigDialogBase.cc \
CounterDialog.cc \
CounterList.cc \
CustomComboBox.cc \
CustomDialog.cc \
CustomFileDialog.cc \
CustomLineEdit.cc \
CustomListBox.cc \
CustomListView.cc \
CustomMainWindow.cc \
CustomPixmap.cc \
CustomProcess.cc \
CustomSlider.cc \
CustomTabWidget.cc \
CustomTextEdit.cc \
CustomToolBar.cc \
CustomToolButton.cc \
DebugMenu.cc \
DockPanel.cc \
ErrorHandler.cc \
FindDialog.cc \
FontEdit.cc \
HelpDialog.cc \
HelpItem.cc \
HelpManager.cc \
IconBrowsedButton.cc \
IconEngine.cc \
ListViewConfig.cc \
OpenPreviousMenu.cc \
OptionFontInfo.cc \
OptionListBox.cc \
QtUtil.cc \
RemoveLineBuffer.cc \
ReplaceDialog.cc \
SelectLineDialog.cc \
StatusBar.cc \
TextSeparator.cc \
ValidFileThread.cc \
XmlFileList.cc \
XmlFileRecord.cc \
XmlOption.cc \
XmlOptions.cc \
XmlTextFormatBlock.cc \
XmlTimeStamp.cc

CONFIG += qt
QT += xml
