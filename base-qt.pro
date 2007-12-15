TEMPLATE = lib
CONFIG = staticlib qt
QT += xml

INCLUDEPATH += ../base
DEPENDPATH += . ../base

HEADERS = \
BaseConfigurationDialog.h \
BaseIcons.h \
BlockHighlight.h \
BoxSelection.h \
BrowsedLineEdit.h \
ClockLabel.h \
ColorDisplay.h \
ColorMenu.h \
CounterDialog.h \
CounterList.h \
CustomComboBox.h \
CustomDialog.h \
CustomFileDialog.h \
CustomGridLayout.h \
CustomLineEdit.h \
CustomMainWindow.h \
CustomPixmap.h \
CustomSlider.h \
CustomTabWidget.h \
CustomTextDocument.h \
CustomTextEdit.h \
CustomToolBar.h \
CustomToolButton.h \
DebugMenu.h \
DockPanel.h \
ErrorHandler.h \
FindDialog.h \
FlatStyle.h \
FontEdit.h \
FontInfo.h \
IconBrowsedButton.h \
IconEngine.h \
IconProvider.h \
ListWidget.h \
MultipleClickCounter.h \
OpenPreviousMenu.h \
OptionBrowsedLineEdit.h \
OptionCheckBox.h \
OptionColorDisplay.h \
OptionComboBox.h \
OptionDialog.h \
OptionFontEdit.h \
OptionFontInfo.h \
OptionIconBrowsedButton.h \
OptionLineEdit.h \
OptionListBox.h \
OptionSlider.h \
OptionSpinBox.h \
OptionWidget.h \
OptionWidgetList.h \
ProcessDialog.h \
ProcessFrame.h \
QtUtil.h \
RemoveLineBuffer.h \
ReplaceDialog.h \
SelectLineDialog.h \
StatusBar.h \
SystemEnvironmentDialog.h \
TextBlockData.h \
TextBlockFlags.h \
TextSelection.h \
TextSeparator.h \
TreeViewConfiguration.h \
TreeView.h \
TreeWidget.h \
ValidFileThread.h \
XmlError.h \
XmlFileList.h \
XmlFileRecord.h \
XmlOption.h \
XmlOptions.h \
XmlTextFormatBlock.h \
XmlTimeStamp.h


SOURCES = \
BaseConfigurationDialog.cpp \
BlockHighlight.cpp \
BoxSelection.cpp \
BrowsedLineEdit.cpp \
ClockLabel.cpp \
ColorDisplay.cpp \
ColorMenu.cpp \
CounterDialog.cpp \
CounterList.cpp \
CustomComboBox.cpp \
CustomDialog.cpp \
CustomFileDialog.cpp \
CustomLineEdit.cpp \
CustomMainWindow.cpp \
CustomPixmap.cpp \
CustomSlider.cpp \
CustomTabWidget.cpp \
CustomTextEdit.cpp \
CustomToolBar.cpp \
CustomToolButton.cpp \
DebugMenu.cpp \
DockPanel.cpp \
ErrorHandler.cpp \
FindDialog.cpp \
FlatStyle.cpp \
FontEdit.cpp \
FontInfo.cpp \
IconBrowsedButton.cpp \
IconEngine.cpp \
IconProvider.cpp \
ListWidget.cpp \
OpenPreviousMenu.cpp \
OptionDialog.cpp \
OptionListBox.cpp \
ProcessDialog.cpp \
ProcessFrame.cpp \
QtUtil.cpp \
RemoveLineBuffer.cpp \
ReplaceDialog.cpp \
SelectLineDialog.cpp \
StatusBar.cpp \
SystemEnvironmentDialog.cpp \
TextSeparator.cpp \
TreeViewConfiguration.cpp \
TreeView.cpp \
TreeWidget.cpp \
ValidFileThread.cpp \
XmlFileList.cpp \
XmlFileRecord.cpp \
XmlOption.cpp \
XmlOptions.cpp \
XmlTextFormatBlock.cpp \
XmlTimeStamp.cpp
