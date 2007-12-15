AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base-qt.pro

INCLUDES =  \
  -I$(qt_includes) \
  -I$(qt_includes)/Qt \
  -I$(qt_includes)/QtCore  \
  -I$(qt_includes)/QtXml \
  -I$(qt_includes)/QtGui \
  -I$(top_srcdir)/base
  
noinst_HEADERS = \
  BaseConfigurationDialog.h \
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
  ListWidget.h \
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
  BaseIcons.h \
  ErrorHandler.h \
  FindDialog.h \
  FlatStyle.h \
  FontEdit.h \
  FontInfo.h \
  IconBrowsedButton.h \
  IconEngine.h \
  IconProvider.h \
  ItemDelegate.h \
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
  TreeView.h \
  TreeViewConfiguration.h \
  TreeWidget.h \
  ValidFileThread.h \
  XmlError.h \
  XmlFileList.h \
  XmlFileRecord.h \
  XmlOption.h \
  XmlOptions.h \
  XmlTextFormatBlock.h \
  XmlTimeStamp.h

noinst_LIBRARIES = libbase-qt.a

libbase_qt_a_SOURCES = \
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
  ListWidget.cpp \
  CustomMainWindow.cpp \
  CustomPixmap.cpp \
  CustomSlider.cpp \
  CustomTextEdit.cpp \
  CustomTabWidget.cpp \
  CustomToolBar.cpp \
  CustomToolButton.cpp \
  DebugMenu.cpp \
  DockPanel.cpp \
  ErrorHandler.cpp \
  FlatStyle.cpp \
  FindDialog.cpp \
  FontEdit.cpp \
  FontInfo.cpp \
  IconBrowsedButton.cpp \
  IconEngine.cpp \
  IconProvider.cpp \
  ItemDelegate.cpp \
  OptionDialog.cpp \
  OpenPreviousMenu.cpp \
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
  TreeView.cpp \
  TreeWidget.cpp \
  TreeViewConfiguration.cpp \
  ValidFileThread.cpp \
  XmlFileList.cpp \
  XmlFileRecord.cpp \
  XmlOption.cpp \
  XmlOptions.cpp \
  XmlTextFormatBlock.cpp \
  XmlTimeStamp.cpp     

nodist_libbase_qt_a_SOURCES = \
  moc_BaseConfigurationDialog.cpp \
  moc_BlockHighlight.cpp \
  moc_BrowsedLineEdit.cpp \
  moc_ColorDisplay.cpp \
  moc_ColorMenu.cpp \
  moc_ClockLabel.cpp \
  moc_CounterDialog.cpp \
  moc_CustomComboBox.cpp \
  moc_CustomFileDialog.cpp \
  moc_CustomLineEdit.cpp \
  moc_ListWidget.cpp \
  moc_CustomMainWindow.cpp \
  moc_CustomSlider.cpp \
  moc_CustomTabWidget.cpp \
  moc_CustomTextEdit.cpp \
  moc_CustomToolBar.cpp \
  moc_CustomToolButton.cpp \
  moc_DebugMenu.cpp \
  moc_DockPanel.cpp \
  moc_FindDialog.cpp \
  moc_FontEdit.cpp \
  moc_IconBrowsedButton.cpp \
  moc_MultipleClickCounter.cpp \
  moc_OpenPreviousMenu.cpp \
  moc_OptionListBox.cpp \
  moc_ProcessFrame.cpp \
  moc_RemoveLineBuffer.cpp \
  moc_ReplaceDialog.cpp \
  moc_SelectLineDialog.cpp \
  moc_StatusBar.cpp \
  moc_TreeView.cpp \
  moc_TreeViewConfiguration.cpp \
  moc_TreeWidget.cpp

clean-local:
	rm -f moc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@
