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
	BrowsedLineEdit.h \
	ClockLabel.h \
	ColorDisplay.h \
	ConfigDialogBase.h \
	CounterDialog.h \
	CounterList.h \
	CustomDialog.h \
	CustomFileDialog.h \
	CustomLineEdit.h \
	CustomListBox.h \
	CustomListView.h \
  CustomMainWindow.h \
	CustomPixmap.h \
	CustomProcess.h \
	CustomSlider.h \
	CustomTabWidget.h \
	CustomTextEdit.h \
	CustomToolButton.h \
	DebugMenu.h \
	DockPanel.h \
	ErrorHandler.h \
	FindDialog.h \
	FontEdit.h \
	Help.h \
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
	ReplaceDialog.h \
	SelectLineDialog.h \
	StateFrame.h \
  TextSelection.h \
  TextSeparator.h \
  ValidFileThread.h \
	XmlError.h \
	XmlFileList.h \
	XmlFileRecord.h \
	XmlOption.h \
	XmlOptions.h \
	XmlTimeStamp.h

noinst_LIBRARIES = libbase-qt.a

libbase_qt_a_SOURCES = \
  BrowsedLineEdit.cc \
  ClockLabel.cc \
  ColorDisplay.cc \
  ConfigDialogBase.cc \
  CounterDialog.cc \
  CounterList.cc \
  CustomDialog.cc \
  CustomFileDialog.cc \
  CustomLineEdit.cc \
  CustomListBox.cc \
  CustomListView.cc \
  CustomPixmap.cc \
	CustomProcess.cc \
  CustomSlider.cc \
	CustomTextEdit.cc \
	CustomTabWidget.cc \
	CustomToolButton.cc \
  DebugMenu.cc \
	DockPanel.cc \
  ErrorHandler.cc \
  FindDialog.cc \
  FontEdit.cc \
	Help.cc \
  IconBrowsedButton.cc \
	IconEngine.cc \
  ListViewConfig.cc \
  OpenPreviousMenu.cc \
  OptionListBox.cc \
  QtUtil.cc \
	ReplaceDialog.cc \
	SelectLineDialog.cc \
  StateFrame.cc \
  TextSeparator.cc \
  ValidFileThread.cc \
	XmlFileList.cc \
	XmlFileRecord.cc \
  XmlOption.cc \
  XmlOptions.cc \
  XmlTimeStamp.cc     

nodist_libbase_qt_a_SOURCES = \
  moc_BrowsedLineEdit.cc \
  moc_ColorDisplay.cc \
  moc_ClockLabel.cc \
  moc_ConfigDialogBase.cc \
  moc_CounterDialog.cc \
  moc_CustomFileDialog.cc \
  moc_CustomLineEdit.cc \
  moc_CustomListBox.cc \
  moc_CustomListView.cc \
  moc_CustomMainWindow.cc \
	moc_CustomProcess.cc \
  moc_CustomSlider.cc \
	moc_CustomTabWidget.cc \
	moc_CustomTextEdit.cc \
  moc_CustomToolButton.cc \
  moc_DebugMenu.cc \
	moc_DockPanel.cc \
  moc_FindDialog.cc \
  moc_FontEdit.cc \
	moc_Help.cc \
  moc_IconBrowsedButton.cc \
  moc_ListViewConfig.cc \
  moc_MultipleClickCounter.cc \
  moc_OpenPreviousMenu.cc \
  moc_OptionListBox.cc \
	moc_ReplaceDialog.cc \
  moc_SelectLineDialog.cc \
  moc_StateFrame.cc

clean-local:
	 rm -f moc_*.cc

### rule to generate qt meta objects
moc_%.cc: %.h
	$(qt_moc) $^ -o $@
