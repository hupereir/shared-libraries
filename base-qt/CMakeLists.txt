# $Id$
project(BASE_QT)
set(BASE_QT_VERSION_MAJOR 1)
set(BASE_QT_VERSION_MINOR 0)
set(BASE_QT_VERSION_PATCH 0)
set(BASE_QT_VERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR}.${BASE_QT_VERSION_PATCH})
set(BASE_QT_SOVERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR})

########### Qt configuration #########
if(USE_QT5)

  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Network REQUIRED)
  find_package(Qt5Xml REQUIRED)
  find_package(Qt5PrintSupport REQUIRED)

  if(WIN32)
    find_package(Qt5WinExtras REQUIRED)
  endif()

  # needed to access QPA classes
  include_directories(${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

else()

  find_package(Qt4 REQUIRED)
  set(QT_USE_QTNETWORK TRUE)
  set(QT_USE_QTXML TRUE)
  include(${QT_USE_FILE})
  add_definitions(${QT_DEFINITIONS})

endif()

########### X11 #########
if(UNIX AND NOT APPLE)

  find_package(XCB COMPONENTS XCB KEYSYMS)
  find_package(X11_XCB)
  if( XCB_XCB_FOUND AND XCB_KEYSYMS_FOUND AND X11_XCB_FOUND )

    add_definitions(-DHAVE_XCB=1)

    if(USE_QT5)
      find_package(Qt5X11Extras REQUIRED)
    endif()

  else()
    add_definitions(-DHAVE_XCB=0)
  endif()

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
  ColorMenu.cpp
  ColorOptionListBox.cpp
  ColorOptionModel.cpp
  ColumnSelectionMenu.cpp
  ColumnSortingMenu.cpp
  ContextMenu.cpp
  CounterDialog.cpp
  CounterModel.cpp
  CursorMonitor.cpp
  CustomComboBox.cpp
  CustomDialog.cpp
  CustomMenu.cpp
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
  RoundedRegion.cpp
  ScratchFileMonitor.cpp
  ScratchFileRemoveDialog.cpp
  SelectLineDialog.cpp
  SelectLineWidget.cpp
  SimpleListView.cpp
  StandardAction.cpp
  SystemEnvironmentDialog.cpp
  TabbedDialog.cpp
  TabWidget.cpp
  TextEditor.cpp
  TextEditorMarginWidget.cpp
  TextEditionDelegate.cpp
  TextEncodingDialog.cpp
  TextEncodingMenu.cpp
  TextEncodingString.cpp
  TextEncodingWidget.cpp
  TextPosition.cpp
  TextSeparator.cpp
  ToolBarMenu.cpp
  ToolButtonStyleMenu.cpp
  TreeItemBase.cpp
  TreeView.cpp
  TreeViewConfiguration.cpp
  TreeViewItemDelegate.cpp
  UserSelectionFrame.cpp
  ValidFileThread.cpp
  WarningDialog.cpp
  WidgetDragMonitor.cpp
  WidgetMonitor.cpp
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

if(XCB_FOUND)

  target_link_libraries(base-qt ${XCB_LIBRARIES})
  target_link_libraries(base-qt ${X11_XCB_LIBRARIES})

endif()

target_link_libraries(base-qt ${QT_LIBRARIES} base)

if(USE_QT5)

  target_link_libraries(base-qt Qt5::Widgets Qt5::Network Qt5::PrintSupport Qt5::Xml)
  if(WIN32)
    target_link_libraries(base-qt Qt5::WinExtras)
  endif()

  if(XCB_FOUND)
    target_link_libraries(base-qt Qt5::X11Extras)
  endif()

endif()
