# $Id$
project(BASE_QT)
set(BASE_QT_VERSION_MAJOR 1)
set(BASE_QT_VERSION_MINOR 0)
set(BASE_QT_VERSION_PATCH 0)
set(BASE_QT_VERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR}.${BASE_QT_VERSION_PATCH})
set(BASE_QT_SOVERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT6)
find_package(Qt6 COMPONENTS Widgets Network Xml PrintSupport Core5Compat REQUIRED)
else()
find_package(Qt5 COMPONENTS Widgets Network Xml PrintSupport REQUIRED)
endif()

if(WIN32)
if(USE_QT6)
  find_package( QT6 COMPONENTS GuiPrivate REQUIRED )
else()
  find_package(Qt5WinExtras REQUIRED)
endif()
endif()

# needed to access QPA classes
if(NOT USE_QT6)
  include_directories(${Qt5Widgets_PRIVATE_INCLUDE_DIRS})
endif()

########### X11 #########
if(UNIX AND NOT APPLE)

  find_package(XCB COMPONENTS XCB KEYSYMS)
  find_package(X11_XCB)
  if( XCB_XCB_FOUND AND XCB_KEYSYMS_FOUND AND X11_XCB_FOUND )
    add_definitions(-DWITH_XCB=1)
    if(NOT USE_QT6)
      find_package(Qt5X11Extras REQUIRED)
    endif()
  else()
    add_definitions(-DWITH_XCB=0)
  endif()
else()
  add_definitions(-DWITH_XCB=0)
endif()

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_qt_SOURCES
  AbstractFindWidget.cpp
  ApplicationMenu.cpp
  BaseContextMenu.cpp
  BaseFileIconProvider.cpp
  BaseFileInfo.cpp
  BaseFileInfoConfigurationWidget.cpp
  BaseFileInfoIconView.cpp
  BaseFileInfoToolTipWidget.cpp
  BaseFileInfoItemDelegate.cpp
  BaseFileInformationDialog.cpp
  BaseFindDialog.cpp
  BaseFindWidget.cpp
  BaseMainWindow.cpp
  BaseConfigurationDialog.cpp
  BaseDialog.cpp
  BaseMenu.cpp
  BasePrintHelper.cpp
  BaseProgressBar.cpp
  BaseReplaceDialog.cpp
  BaseReplaceWidget.cpp
  BaseSocketInterface.cpp
  BaseStatusBar.cpp
  BaseToolTipWidget.cpp
  BlockHighlight.cpp
  BoxSelection.cpp
  BrowsedLineEditor.cpp
  BrowseIconButton.cpp
  BusyWidget.cpp
  ClockLabel.cpp
  ColorComboBox.cpp
  ColorDisplay.cpp
  ColorGrabButton.cpp
  ColorGrabObject.cpp
  ColorOptionListBox.cpp
  ColorOptionModel.cpp
  ColumnSelectionMenu.cpp
  ColumnSortingMenu.cpp
  ComboBox.cpp
  ContextMenu.cpp
  CounterDialog.cpp
  CounterModel.cpp
  CursorMonitor.cpp
  DebugMenu.cpp
  DefaultFolders.cpp
  DetailsDialog.cpp
  Dialog.cpp
  DockPanel.cpp
  DockWidget.cpp
  DragMonitor.cpp
  ElidedLabel.cpp
  ErrorHandler.cpp
  EmbeddedWidget.cpp
  FileDialog.cpp
  FileList.cpp
  FilePermissionsWidget.cpp
  FileRecordModel.cpp
  FileRecordToolTipWidget.cpp
  FileSystemWatcher.cpp
  FontEditor.cpp
  FontInfo.cpp
  GridLayout.cpp
  GridLayoutItem.cpp
  HtmlDialog.cpp
  HtmlTextNode.cpp
  IconCacheDialog.cpp
  IconCacheModel.cpp
  IconEngine.cpp
  IconSize.cpp
  IconSizeComboBox.cpp
  IconSizeMenu.cpp
  IconView.cpp
  IconViewItem.cpp
  ImageFileDialog.cpp
  InformationDialog.cpp
  InterruptionHandler.cpp
  ItemModel.cpp
  KeyModifier.cpp
  LineEditor.cpp
  LineNumberDisplay.cpp
  LogWidget.cpp
  MessageWidget.cpp
  MimeTypeIconProvider.cpp
  MultipleClickCounter.cpp
  NetworkConnectionMonitor.cpp
  OpenWithComboBox.cpp
  OpenWithDialog.cpp
  OptionBrowsedLineEditor.cpp
  OptionCheckBox.cpp
  OptionColorDisplay.cpp
  OptionComboBox.cpp
  OptionDialog.cpp
  OptionFontEditor.cpp
  OptionFontInfo.cpp
  OptionIconSizeComboBox.cpp
  OptionLineEditor.cpp
  OptionListBox.cpp
  OptionModel.cpp
  OptionRadioButton.cpp
  OptionSlider.cpp
  OptionSpinBox.cpp
  PasswordEditor.cpp
  PathEditor.cpp
  PathHistory.cpp
  PathHistoryConfiguration.cpp
  Pixmap.cpp
  PixmapEngine.cpp
  PrinterOptionWidget.cpp
  PrintPreviewDialog.cpp
  ProgressStatusBar.cpp
  QuestionDialog.cpp
  QtUtil.cpp
  RecentFilesConfiguration.cpp
  RecentFilesMenu.cpp
  RemoveLineBuffer.cpp
  ReverseOrderAction.cpp
  RoundedRegion.cpp
  ScratchFileMonitor.cpp
  ScratchFileRemoveDialog.cpp
  ScrollBarMonitor.cpp
  SelectLineDialog.cpp
  SelectLineWidget.cpp
  SingleTimeDialog.cpp
  SimpleListView.cpp
  Slider.cpp
  StandardAction.cpp
  SystemEnvironmentDialog.cpp
  TabbedDialog.cpp
  TabWidget.cpp
  TextDocument.cpp
  TextEditor.cpp
  TextEditorMarginWidget.cpp
  TextEditionDelegate.cpp
  TextEncodingDialog.cpp
  TextEncodingMenu.cpp
  TextEncodingString.cpp
  TextEncodingWidget.cpp
  TextPosition.cpp
  TextSeparator.cpp
  ToolBar.cpp
  ToolBarMenu.cpp
  ToolButton.cpp
  ToolButtonStyleMenu.cpp
  TreeItemBase.cpp
  TreeView.cpp
  TreeViewConfiguration.cpp
  TreeViewItemDelegate.cpp
  UserSelectionFrame.cpp
  ValidFileThread.cpp
  WaylandUtil.cpp
  WarningDialog.cpp
  WidgetDragMonitor.cpp
  WidgetMonitor.cpp
  WindowManager.cpp
  WinUtil.cpp
  XcbUtil.cpp
  XmlColor.cpp
  XmlCommandLineArguments.cpp
  XmlDocument.cpp
  XmlFileList.cpp
  XmlFileRecord.cpp
  XmlOption.cpp
  XmlOptions.cpp
  XmlPathHistory.cpp
  XmlTextFormatBlock.cpp
  XmlTimeStamp.cpp
  basePixmaps.qrc
)

set(base_qt_RESOURCES basePixmaps.qrc)

if(USE_SHARED_LIBS)
  add_library(base-qt SHARED ${base_qt_SOURCES} ${base_qt_RESOURCES})
  set_target_properties(base-qt PROPERTIES VERSION ${BASE_QT_VERSION} SOVERSION ${BASE_QT_SOVERSION})
  install(TARGETS base-qt DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
  add_library(base-qt STATIC ${base_qt_SOURCES} ${base_qt_RESOURCES})
endif()

target_link_libraries(base-qt base)
if(XCB_FOUND)
  target_link_libraries(base-qt ${XCB_LIBRARIES})
  target_link_libraries(base-qt ${X11_XCB_LIBRARIES})
endif()

target_link_libraries(base-qt Qt::Widgets Qt::Network Qt::PrintSupport Qt::Xml)

if(USE_QT6)
  # needed for QTextCodec
  target_link_libraries(base-qt Qt::Core5Compat)
endif()

if(WIN32)
  target_link_libraries(base-qt Qt::WinExtras)
endif()

if(XCB_FOUND AND NOT USE_QT6)
  target_link_libraries(base-qt Qt::X11Extras)
endif()
