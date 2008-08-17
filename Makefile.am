AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = base-qt.pro basePixmaps.qrc

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
  BrowsedLineEditor.h \
  ClockLabel.h \
  ColorDisplay.h \
  ColorMenu.h \
  CounterDialog.h \
  CounterModel.h \
  CustomComboBox.h \
  CustomDialog.h \
  CustomFileDialog.h \
  GridLayout.h \
  CustomMainWindow.h \
  CustomPixmap.h \
  CustomSlider.h \
  CustomTextDocument.h \
  CustomToolBar.h \
  CustomToolButton.h \
  DebugMenu.h \
  DockPanel.h \
  BaseIcons.h \
  ErrorHandler.h \
  FileList.h \
  FileRecordBaseProperties.h \
  FileRecordModel.h \
  FindDialog.h \
  FlatStyle.h \
  FontEditor.h \
  FontInfo.h \
  HeaderMenu.h \
  IconBrowsedButton.h \
  IconCacheDialog.h \
  IconCacheModel.h \
  IconEngine.h \
  IconFileDialog.h \
  ItemDelegate.h \
  ItemModel.h \
  LineEditor.h \
  LineNumberDisplay.h \
  ListModel.h \
  ListWidget.h \
  MultipleClickCounter.h \
  OptionBrowsedLineEditor.h \
  OptionCheckBox.h \
  OptionColorDisplay.h \
  OptionComboBox.h \
  OptionDialog.h \
  OptionFontEditor.h \
  OptionFontInfo.h \
  OptionIconBrowsedButton.h \
  OptionLineEditor.h \
  OptionListBox.h \
  OptionRadioButton.h \
  OptionSlider.h \
  OptionSpinBox.h \
  OptionWidget.h \
  OptionWidgetList.h \
  PixmapEngine.h \
  ProcessDialog.h \
  ProcessFrame.h \
  QtUtil.h \
  RecentFilesMenu.h \
  RemoveLineBuffer.h \
  ReplaceDialog.h \
  SelectLineDialog.h \
  StatusBar.h \
  SystemEnvironmentDialog.h \
  TabbedDialog.h \
  TabWidget.h \
  TextEditor.h \
  TextBlockData.h \
  TextBlockFlags.h \
  TextSelection.h \
  TextSeparator.h \
  TreeItem.h \
  TreeItemBase.h \
  TreeModel.h \
  TreeView.h \
  TreeViewConfiguration.h \
  TreeWidget.h \
  UserSelectionFrame.h \
  ValidFileThread.h \
  X11Util.h \
  XmlError.h \
  XmlFileList.h \
  XmlFileRecord.h \
  XmlOption.h \
  XmlOptions.h \
  XmlTextFormatBlock.h \
  XmlTimeStamp.h \
  XmlUtil.h

noinst_LIBRARIES = libbase-qt.a

libbase_qt_a_SOURCES = \
  BaseConfigurationDialog.cpp \
  BlockHighlight.cpp \
  BoxSelection.cpp \
  BrowsedLineEditor.cpp \
  ClockLabel.cpp \
  ColorDisplay.cpp \
  ColorMenu.cpp \
  CounterDialog.cpp \
  CounterModel.cpp \
  CustomComboBox.cpp \
  CustomDialog.cpp \
  CustomFileDialog.cpp \
  CustomMainWindow.cpp \
  CustomPixmap.cpp \
  CustomSlider.cpp \
  CustomToolBar.cpp \
  CustomToolButton.cpp \
  DebugMenu.cpp \
  DockPanel.cpp \
  ErrorHandler.cpp \
  FileList.cpp \
  FileRecordModel.cpp \
  FindDialog.cpp \
  FlatStyle.cpp \
  FontEditor.cpp \
  FontInfo.cpp \
  HeaderMenu.cpp \
  IconBrowsedButton.cpp \
  IconCacheDialog.cpp \
  IconCacheModel.cpp \
  IconEngine.cpp \
  IconFileDialog.cpp \
  ItemDelegate.cpp \
  ItemModel.cpp \
  LineEditor.cpp \
  LineNumberDisplay.cpp \
  ListWidget.cpp \
  OptionDialog.cpp \
  OptionListBox.cpp \
  PixmapEngine.cpp \
  ProcessDialog.cpp \
  ProcessFrame.cpp \
  QtUtil.cpp \
  RecentFilesMenu.cpp \
  RemoveLineBuffer.cpp \
  ReplaceDialog.cpp \
  SelectLineDialog.cpp \
  StatusBar.cpp \
  SystemEnvironmentDialog.cpp \
  TabbedDialog.cpp \
  TabWidget.cpp \
  TextEditor.cpp \
  TextSeparator.cpp \
  TreeItemBase.cpp \
  TreeView.cpp \
  TreeWidget.cpp \
  TreeViewConfiguration.cpp \
  UserSelectionFrame.cpp \
  ValidFileThread.cpp \
  X11Util.cpp \
  XmlFileList.cpp \
  XmlFileRecord.cpp \
  XmlOption.cpp \
  XmlOptions.cpp \
  XmlTextFormatBlock.cpp \
  XmlTimeStamp.cpp \
  XmlUtil.cpp

nodist_libbase_qt_a_SOURCES = \
  qrc_basePixmaps.cpp \
  moc_BaseConfigurationDialog.cpp \
  moc_BlockHighlight.cpp \
  moc_BrowsedLineEditor.cpp \
  moc_ColorDisplay.cpp \
  moc_ColorMenu.cpp \
  moc_ClockLabel.cpp \
  moc_CounterDialog.cpp \
  moc_CustomComboBox.cpp \
  moc_CustomFileDialog.cpp \
  moc_CustomMainWindow.cpp \
  moc_CustomSlider.cpp \
  moc_CustomToolBar.cpp \
  moc_CustomToolButton.cpp \
  moc_DebugMenu.cpp \
  moc_DockPanel.cpp \
  moc_FileList.cpp \
  moc_FindDialog.cpp \
  moc_FontEditor.cpp \
  moc_HeaderMenu.cpp \
  moc_IconBrowsedButton.cpp \
  moc_IconCacheDialog.cpp \
  moc_IconFileDialog.cpp \
  moc_LineEditor.cpp \
  moc_LineNumberDisplay.cpp \
  moc_ListWidget.cpp \
  moc_MultipleClickCounter.cpp \
  moc_OptionListBox.cpp \
  moc_ProcessFrame.cpp \
  moc_RecentFilesMenu.cpp \
  moc_RemoveLineBuffer.cpp \
  moc_ReplaceDialog.cpp \
  moc_SelectLineDialog.cpp \
  moc_StatusBar.cpp \
  moc_TabbedDialog.cpp \
  moc_TabWidget.cpp \
  moc_TextEditor.cpp \
  moc_TreeView.cpp \
  moc_TreeViewConfiguration.cpp \
  moc_TreeWidget.cpp \
  moc_UserSelectionFrame.cpp \
  moc_XmlFileList.cpp

clean-local:
	rm -f moc_*.cpp
	rm -f qrc_*.cpp

### rule to generate qt meta objects
moc_%.cpp: %.h
	$(qt_moc) $^ -o $@

qrc_%.cpp: %.qrc
	$(qt_rcc) -name $* $^ -o $@
