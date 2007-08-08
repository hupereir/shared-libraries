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
  BaseTextHighlight.h \
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
  CustomListBox.h \
  CustomListView.h \
  CustomMainWindow.h \
  CustomPixmap.h \
  CustomProcess.h \
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
  FontEdit.h \
  HelpDialog.h \
  HelpItem.h \
  HelpItemList.h \
  HelpManager.h \
  IconBrowsedButton.h \
  IconEngine.h \
  IconProvider.h \
  ListViewConfiguration.h \
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

noinst_LIBRARIES = libbase-qt.a

libbase_qt_a_SOURCES = \
  BaseConfigurationDialog.cc \
  BaseTextHighlight.cc \
  BlockHighlight.cc \
  BoxSelection.cc \
  BrowsedLineEdit.cc \
  ClockLabel.cc \
  ColorDisplay.cc \
  ColorMenu.cc \
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
  CustomTextEdit.cc \
  CustomTabWidget.cc \
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
  IconProvider.cc \
  ListViewConfiguration.cc \
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

nodist_libbase_qt_a_SOURCES = \
  moc_BaseConfigurationDialog.cc \
  moc_BlockHighlight.cc \
  moc_BrowsedLineEdit.cc \
  moc_ColorDisplay.cc \
  moc_ColorMenu.cc \
  moc_ClockLabel.cc \
  moc_CounterDialog.cc \
  moc_CustomComboBox.cc \
  moc_CustomFileDialog.cc \
  moc_CustomLineEdit.cc \
  moc_CustomListBox.cc \
  moc_CustomListView.cc \
  moc_CustomMainWindow.cc \
  moc_CustomProcess.cc \
  moc_CustomSlider.cc \
  moc_CustomTabWidget.cc \
  moc_CustomTextEdit.cc \
  moc_CustomToolBar.cc \
  moc_CustomToolButton.cc \
  moc_DebugMenu.cc \
  moc_DockPanel.cc \
  moc_FindDialog.cc \
  moc_FontEdit.cc \
  moc_HelpDialog.cc \
  moc_HelpManager.cc \
  moc_IconBrowsedButton.cc \
  moc_ListViewConfiguration.cc \
  moc_MultipleClickCounter.cc \
  moc_OpenPreviousMenu.cc \
  moc_OptionListBox.cc \
  moc_RemoveLineBuffer.cc \
  moc_ReplaceDialog.cc \
  moc_SelectLineDialog.cc \
  moc_StatusBar.cc

clean-local:
	rm -f moc_*.cc

### rule to generate qt meta objects
moc_%.cc: %.h
	$(qt_moc) $^ -o $@
