# $Id$
PROJECT( BASE_QT )
SET( BASE_QT_VERSION_MAJOR 1 )
SET( BASE_QT_VERSION_MINOR 0 )
SET( BASE_QT_VERSION_PATCH 0 )
SET( BASE_QT_VERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR}.${BASE_QT_VERSION_PATCH} )
SET( BASE_QT_SOVERSION ${BASE_QT_VERSION_MAJOR}.${BASE_QT_VERSION_MINOR} )

########### X11 #########
IF( UNIX AND NOT APPLE )

  FIND_PACKAGE( X11 )
  IF( X11_FOUND )
    INCLUDE_DIRECTORIES( ${X11_X11_INCLUDE_PATH} )
    ADD_DEFINITIONS( -DQ5_WS_X11 )
  ENDIF()

ENDIF()

########### Qt4 configuration #########
IF( USE_QT5 )

  FIND_PACKAGE( Qt5Widgets REQUIRED )
  FIND_PACKAGE( Qt5Gui REQUIRED )

  # needed to access QPA classes
  INCLUDE_DIRECTORIES(${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

ELSE()

  FIND_PACKAGE( Qt4 REQUIRED )
  SET( QT_USE_QTXML TRUE )
  INCLUDE( ${QT_USE_FILE} )
  ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ENDIF()

########### includes ###############
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/base)

########### links #########
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES( ${CMAKE_INSTALL_PREFIX}/lib )

########### next target ###############
SET( base_qt_SOURCES
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
  CustomToolBar.cpp
  CustomToolButton.cpp
  DebugMenu.cpp
  DetailsDialog.cpp
  DockPanel.cpp
  DockWidget.cpp
  ElidedLabel.cpp
  ErrorHandler.cpp
  FileDialog.cpp
  FileList.cpp
  FilePermissionsWidget.cpp
  FileRecordModel.cpp
  FileSystemWatcher.cpp
  FontEditor.cpp
  FontInfo.cpp
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
  OptionDialog.cpp
  OptionListBox.cpp
  OptionModel.cpp
  OptionWidgetList.cpp
  PathEditor.cpp
  PathHistory.cpp
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
  TextPosition.cpp
  TextSeparator.cpp
  ToolBarMenu.cpp
  ToolButtonStyleMenu.cpp
  TransitionWidget.cpp
  TreeItemBase.cpp
  TreeView.cpp
  TreeViewConfiguration.cpp
  UserSelectionFrame.cpp
  ValidFileThread.cpp
  WarningDialog.cpp
  WidgetMonitor.cpp
  WinUtil.cpp
  X11Util.cpp
  XmlColor.cpp
  XmlCommandLineArguments.cpp
  XmlDocument.cpp
  XmlFileList.cpp
  XmlFileRecord.cpp
  XmlMigration.cpp
  XmlOption.cpp
  XmlOptions.cpp
  XmlTextFormatBlock.cpp
  XmlTimeStamp.cpp
  XmlString.cpp
)

SET( base_qt_HEADERS
  AnimatedLabel.h
  AnimatedLineEditor.h
  AnimatedStackedWidget.h
  AnimatedTabWidget.h
  AnimatedTreeView.h
  BaseConfigurationDialog.h
  BaseDialog.h
  BaseFindDialog.h
  BaseFileInfoToolTipWidget.h
  BaseMainWindow.h
  BasePrintHelper.h
  BaseProgressBar.h
  BaseReplaceDialog.h
  BaseStatusBar.h
  BaseToolTipWidget.h
  BlockHighlight.h
  BrowsedLineEditor.h
  BrowseIconButton.h
  BusyWidget.h
  ColorDisplay.h
  ColorGrabButton.h
  ColorGrabObject.h
  ColorMenu.h
  ColumnSelectionMenu.h
  ColumnSortingMenu.h
  ClockLabel.h
  ContextMenu.h
  CounterDialog.h
  CustomComboBox.h
  CustomSlider.h
  CustomTextDocument.h
  CustomToolBar.h
  CustomToolButton.h
  DebugMenu.h
  DetailsDialog.h
  DockPanel.h
  DockWidget.h
  ElidedLabel.h
  FileList.h
  FileRecordModel.h
  FileSystemWatcher.h
  FontEditor.h
  HtmlDialog.h
  IconCacheDialog.h
  IconSizeMenu.h
  IconView.h
  ImageFileDialog.h
  ItemModel.h
  LineEditor.h
  LineNumberDisplay.h
  LogWidget.h
  OptionDialog.h
  OptionListBox.h
  OptionModel.h
  PathEditor.h
  PathEditor_p.h
  PlacesWidget.h
  PlacesWidget_p.h
  PrinterOptionWidget.h
  PrintPreviewDialog.h
  PrintPreviewDialog_p.h
  ProgressStatusBar.h
  RecentFilesConfiguration.h
  RecentFilesMenu.h
  RemoveLineBuffer.h
  ScratchFileMonitor.h
  ScratchFileRemoveDialog.h
  ScrollObject.h
  SelectLineDialog.h
  SimpleListView.h
  TabbedDialog.h
  TabWidget.h
  TextEditor.h
  TextEditorMarginWidget.h
  ToolButtonStyleMenu.h
  TransitionWidget.h
  TreeView.h
  TreeViewItemDelegate.h
  UserSelectionFrame.h
  ValidFileThread.h
  XmlFileList.h
)

SET( base_qt_RESOURCES basePixmaps.qrc)

IF( USE_QT5 )

  QT5_WRAP_CPP( base_qt_HEADERS_MOC ${base_qt_HEADERS} )
  QT5_ADD_RESOURCES( base_qt_RESOURCES_RCC ${base_qt_RESOURCES})

ELSE()

  QT4_WRAP_CPP( base_qt_HEADERS_MOC ${base_qt_HEADERS} )
  QT4_ADD_RESOURCES( base_qt_RESOURCES_RCC ${base_qt_RESOURCES})

ENDIF()


IF( ENABLE_SHARED )

  ADD_LIBRARY( base-qt SHARED
    ${base_qt_SOURCES}
    ${base_qt_HEADERS_MOC}
    ${base_qt_RESOURCES_RCC}
  )

  SET_TARGET_PROPERTIES( base-qt PROPERTIES VERSION ${BASE_QT_VERSION} SOVERSION ${BASE_QT_SOVERSION} )
  TARGET_LINK_LIBRARIES( base-qt ${QT_LIBRARIES} base )
  INSTALL( TARGETS base-qt DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

ELSE()

  ADD_LIBRARY( base-qt STATIC
    ${base_qt_SOURCES}
    ${base_qt_HEADERS_MOC}
    ${base_qt_RESOURCES_RCC}
  )

  IF( X11_FOUND )

    TARGET_LINK_LIBRARIES( base-qt ${X11_X11_LIB} )

  ENDIF()

  TARGET_LINK_LIBRARIES( base-qt ${QT_LIBRARIES} base )

ENDIF()

IF( USE_QT5 )

  QT5_USE_MODULES( base-qt Gui Widgets Xml PrintSupport )

ENDIF()
