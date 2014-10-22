# $Id$
project(BASE_QT)
set(BASE_QT_VERSION_MAJOR 1)
set(BASE_QT_VERSION_MINOR 0)
set(BASE_QT_VERSION_PATCH 0)
set(BASE_QT_VERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR}.${BASE_QT_VERSION_PATCH})
set(BASE_QT_SOVERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT5)

  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Xml REQUIRED)
  find_package(Qt5PrintSupport REQUIRED)

  # needed to access QPA classes
  include_directories(${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

else()

  find_package(Qt4 REQUIRED)
  set(QT_USE_QTXML TRUE)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

endif()

########### X11 #########
if(UNIX AND NOT APPLE)

  find_package(X11)
  if(X11_FOUND)
     find_package(XCB REQUIRED COMPONENTS XCB KEYSYMS)
     find_package(X11_XCB REQUIRED)
     include_directories(${X11_X11_INCLUDE_PATH})
  endif()

  add_definitions(-DHAVE_XCB=${X11_FOUND})

else()

  add_definitions(-DHAVE_XCB=0)

endif()

########### includes ###############
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/base)

########### links #########
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

########### next target ###############
set(base_qt_SOURCES
  AnimatedLabel.cpp
  AnimatedLineEditor.cpp
  AnimatedStackedWidget.cpp
  AnimatedTabWidget.cpp
  AnimatedTextEditor.cpp
  AnimatedTreeView.cpp
  BaseContextMenu.cpp
  BaseFileIconProvider.cpp
  BaseFileInfo.cpp
  BaseFileInfoConfigurationWidget.cpp
  BaseFileInfoToolTipWidget.cpp
  BaseFileInformationDialog.cpp
  BaseFindDialog.cpp
  BaseMainWindow.cpp
  BaseConfigurationDialog.cpp
  BaseDialog.cpp
  BasePrintHelper.cpp
  BaseProgressBar.cpp
  BaseReplaceDialog.cpp
  BaseStatusBar.cpp
  BaseToolTipWidget.cpp
  BlockHighlight.cpp
  BoxSelection.cpp
  BrowsedLineEditor.cpp
  BrowseIconButton.cpp
  BusyWidget.cpp
  ClockLabel.cpp
  ColorDisplay.cpp
  ColorGrabButton.cpp
  ColorGrabObject.cpp
  ColorMenu.cpp
  ColumnSelectionMenu.cpp
  ColumnSortingMenu.cpp
  ContextMenu.cpp
  CounterDialog.cpp
  CounterModel.cpp
  CursorMonitor.cpp
  CustomComboBox.cpp
  CustomDialog.cpp
  CustomPixmap.cpp
  CustomSlider.cpp
  CustomTextDocument.cpp
  CustomToolBar.cpp
  CustomToolButton.cpp
  DebugMenu.cpp
  DefaultFolders.cpp
  DetailsDialog.cpp
  DockPanel.cpp
  DockWidget.cpp
  DragMonitor.cpp
  ElidedLabel.cpp
  ErrorHandler.cpp
  FileDialog.cpp
  FileList.cpp
  FilePermissionsWidget.cpp
  FileRecordModel.cpp
  FileSystemWatcher.cpp
  FontEditor.cpp
  FontInfo.cpp
  GridLayoutItem.cpp
  HtmlDialog.cpp
  HtmlTextNode.cpp
  IconCacheDialog.cpp
  IconCacheModel.cpp
  IconEngine.cpp
  IconSize.cpp
  IconSizeMenu.cpp
  IconView.cpp
  IconViewItem.cpp
  ImageFileDialog.cpp
  InformationDialog.cpp
  ItemModel.cpp
  KeyModifier.cpp
  LineEditor.cpp
  LineNumberDisplay.cpp
  LogWidget.cpp
  MimeTypeIconProvider.cpp
  MultipleClickCounter.cpp
  OptionBrowsedLineEditor.cpp
  OptionCheckBox.cpp
  OptionColorDisplay.cpp
  OptionComboBox.cpp
  OptionDialog.cpp
  OptionFontEditor.cpp
  OptionFontInfo.cpp
  OptionLineEditor.cpp
  OptionListBox.cpp
  OptionModel.cpp
  OptionRadioButton.cpp
  OptionSlider.cpp
  OptionSpinBox.cpp
  OptionWidgetList.cpp
  PathEditor.cpp
  PathHistory.cpp
  PathHistoryConfiguration.cpp
  PixmapEngine.cpp
  PlacesWidget.cpp
  PrinterOptionWidget.cpp
  PrintPreviewDialog.cpp
  ProgressStatusBar.cpp
  QuestionDialog.cpp
  QtUtil.cpp
  RecentFilesConfiguration.cpp
  RecentFilesMenu.cpp
  RemoveLineBuffer.cpp
  RoundedPath.cpp
  RoundedRegion.cpp
  ScratchFileMonitor.cpp
  ScratchFileRemoveDialog.cpp
  ScrollObject.cpp
  SelectLineDialog.cpp
  SimpleListView.cpp
  SystemEnvironmentDialog.cpp
  TabbedDialog.cpp
  TabWidget.cpp
  TextEditor.cpp
  TextEditorMarginWidget.cpp
  TextEditionDelegate.cpp
  TextEncodingDialog.cpp
  TextEncodingMenu.cpp
  TextEncodingWidget.cpp
  TextPosition.cpp
  TextSeparator.cpp
  ToolBarMenu.cpp
  ToolButtonStyleMenu.cpp
  TransitionWidget.cpp
  TreeItemBase.cpp
  TreeView.cpp
  TreeViewConfiguration.cpp
  TreeViewItemDelegate.cpp
  UserSelectionFrame.cpp
  ValidFileThread.cpp
  WarningDialog.cpp
  WidgetMonitor.cpp
  WinUtil.cpp
  XcbUtil.cpp
  XmlColor.cpp
  XmlCommandLineArguments.cpp
  XmlDocument.cpp
  XmlFileList.cpp
  XmlFileRecord.cpp
  XmlMigration.cpp
  XmlOption.cpp
  XmlOptions.cpp
  XmlPathHistory.cpp
  XmlTextFormatBlock.cpp
  XmlTimeStamp.cpp
  XmlString.cpp
)

set(base_qt_RESOURCES basePixmaps.qrc)

if(USE_QT5)

  qt5_add_resources(base_qt_RESOURCES_RCC ${base_qt_RESOURCES})

else()

  qt4_add_resources(base_qt_RESOURCES_RCC ${base_qt_RESOURCES})

endif()


if(ENABLE_SHARED)

  add_library(base-qt SHARED ${base_qt_SOURCES} ${base_qt_RESOURCES_RCC})
  set_target_properties(base-qt PROPERTIES VERSION ${BASE_QT_VERSION} SOVERSION ${BASE_QT_SOVERSION})
  install(TARGETS base-qt DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

else()

  add_library(base-qt STATIC ${base_qt_SOURCES} ${base_qt_RESOURCES_RCC})

endif()

if(X11_FOUND)

  target_link_libraries(base-qt ${X11_X11_LIB})
  target_link_libraries(base-qt ${XCB_LIBRARIES})
  target_link_libraries(base-qt ${X11_XCB_LIBRARIES})

endif()

target_link_libraries(base-qt ${QT_LIBRARIES} base)

if(USE_QT5)

  qt5_use_modules(base-qt Gui Widgets Xml PrintSupport)

endif()
