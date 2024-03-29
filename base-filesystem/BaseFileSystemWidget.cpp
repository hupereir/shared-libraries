/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "BaseFileSystemWidget.h"
#include "BaseFileInfo.h"
#include "BaseFileInfoItemDelegate.h"
#include "ColumnSelectionMenu.h"
#include "ColumnSortingMenu.h"
#include "ContextMenu.h"
#include "FileInformationDialog.h"
#include "FileRecordProperties.h"
#include "FileRecordToolTipWidget.h"
#include "FileSystemIconNames.h"
#include "IconEngine.h"
#include "PathEditor.h"
#include "QtUtil.h"
#include "RemoveFilesDialog.h"
#include "RenameFileDialog.h"
#include "Singleton.h"
#include "ToolBar.h"
#include "ToolBarMenu.h"
#include "TreeView.h"
#include "Util.h"
#include "XmlOptions.h"



#include <QApplication>
#include <QContextMenuEvent>
#include <QDir>
#include <QHeaderView>
#include <QLayout>
#include <QMainWindow>

namespace
{
    namespace Local
    {

        class LocalToolBar: public ToolBar
        {

            Q_OBJECT

            public:

            //* constructor
            LocalToolBar( const QString& title, QWidget* parent, const QString& option ):
                ToolBar( title, parent, option )
            { setTransparent( true ); }

            protected:

            //* context menu
            void contextMenuEvent( QContextMenuEvent* event ) override
            {


                auto mainwindow( qobject_cast<QMainWindow*>( window() ) );
                if( !mainwindow ) return;
                std::unique_ptr<QMenu> menu( mainwindow->createPopupMenu() );

                if( menu )
                {
                    // move and show menu
                    menu->adjustSize();
                    menu->exec( event->globalPos() );
                }
            }


        };

    }
}

#include "BaseFileSystemWidget.moc"

//_____________________________________________
BaseFileSystemWidget::BaseFileSystemWidget( QWidget *parent ):
    QWidget( parent ),
    Counter( QStringLiteral("BaseFileSystemWidget") ),
    sizePropertyId_( FileRecord::PropertyId::get( FileRecordProperties::Size ) ),
    showNavigator_( false ),
    homePath_( Util::home() ),
    fileSystemWatcher_( this ),
    thread_( new FileThread(this) )
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget::BaseFileSystemWidget.\n") );

    auto layout = new QVBoxLayout;
    layout->setSpacing(2);
    QtUtil::setMargin(layout, 0);
    setLayout( layout );

    // toolbar
    auto toolbar = new Local::LocalToolBar( tr( "Navigation Toolbar" ), this, QStringLiteral("NAVIGATION_TOOLBAR") );
    layout->addWidget( toolbar );

    // path editor
    layout->addWidget( pathEditor_ = new PathEditor( this ) );
    pathEditor_->setHomePath( Util::home() );

    // tooltip widget
    toolTipWidget_ = new FileRecordToolTipWidget( this );

    // initialize local root path list
    File::List rootPathList;
    const auto drives = QDir::drives();
    std::transform( drives.begin(), drives.end(), std::back_inserter( rootPathList ),
        []( const QFileInfo& fileInfo ) { return File( fileInfo.path() ); } );

    pathEditor_->setRootPathList( rootPathList );

    connect( pathEditor_, &PathEditor::pathChanged, this, [this](const File&){ _update(); } );
    connect( pathEditor_, &PathEditor::pathChanged, this, &BaseFileSystemWidget::_updateNavigationActions );
    connect( pathEditor_, &PathEditor::pathChanged, this, &BaseFileSystemWidget::_updateFileSystemWatcher );

    // install actions
    _installActions();
    toolbar->addAction( previousDirectoryAction_ );
    toolbar->addAction( nextDirectoryAction_ );
    toolbar->addAction( parentDirectoryAction_ );
    toolbar->addAction( homeDirectoryAction_ );
    toolbar->addAction( workingDirectoryAction_ );
    toolbar->addAction( reloadAction_ );

    // file list
    layout->addWidget( list_ = new TreeView( this ), 1);
    if( list_->itemDelegateForColumn(0) ) list_->itemDelegateForColumn(0)->deleteLater();
    list_->setItemDelegateForColumn(0, new BaseFileInfoItemDelegate( list_ ) );

    list_->setMouseTracking( true );
    list_->setDragEnabled( true );
    list_->setModel( &model_ );
    list_->setSelectionMode( QAbstractItemView::ContiguousSelection );
    list_->setOptionName( QStringLiteral("FILE_SYSTEM_LIST") );
    list_->header()->hide();

    // list menu
    // should move to proper _updateMenu method, and only show relevant actions
    // also, should use baseFileInfo model
    auto menu = new ContextMenu( list_ );
    menu->addMenu( new ColumnSortingMenu( menu, list_ ) );
    menu->addMenu( new ColumnSelectionMenu( menu, list_ ) );
    menu->addSeparator();
    menu->addAction( hiddenFilesAction_ );
    menu->addSeparator();
    menu->addAction( openAction_ );
    menu->addAction( renameAction_ );
    menu->addAction( removeAction_ );
    menu->addSeparator();
    menu->addAction( filePropertiesAction_ );

    // assign
    contextMenu_ = menu;

    // connections
    connect( &model_, &QAbstractItemModel::layoutChanged, list_, &TreeView::updateMask );
    connect( list_->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &BaseFileSystemWidget::_updateActions );
    connect( list_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &BaseFileSystemWidget::_updateActions );
    connect( list_, &QAbstractItemView::activated, this, &BaseFileSystemWidget::_itemActivated );
    connect( list_, &TreeView::hovered, this, &BaseFileSystemWidget::_showToolTip );

    // connect filesystem watcher
    connect( &fileSystemWatcher_, &QFileSystemWatcher::directoryChanged, this, QOverload<const QString&>::of( &BaseFileSystemWidget::_update ) );

    // connect thread
    connect( thread_.get(), &FileThread::filesAvailable, this, &BaseFileSystemWidget::_processFiles );

    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &BaseFileSystemWidget::_updateConfiguration );
    _updateConfiguration();
    _updateNavigationActions();
    _updateActions();

}

//_________________________________________________________
File BaseFileSystemWidget::path() const
{ return pathEditor_->path(); }

//_________________________________________________________
void BaseFileSystemWidget::setHomePath( const File& path )
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::setHomePath.\n") );
    homePath_ = path;
    if( pathEditor_->path().isEmpty() )
    { setPath( path ); }
}

//_________________________________________________________
void BaseFileSystemWidget::setWorkingPath( const File& path )
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::setWorkingPath.\n") );
    workingPath_ = path;
    if( pathEditor_->path().isEmpty() )
    { setPath( path ); }
}

//_________________________________________________________
void BaseFileSystemWidget::clear()
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget::Clear.\n") );
    model_.clear();

}

//_________________________________________________________
void BaseFileSystemWidget::setPath( const File &path, bool forced )
{

    Debug::Throw() << "BaseFileSystemWidget::setPath - path: " << path << Qt::endl;

    // check if changed
    if( forced || pathEditor_->path() != path )
    {
        pathEditor_->setPath( path );
        _update();
        _updateNavigationActions();
        _updateFileSystemWatcher();
    }

}

//______________________________________________________
void BaseFileSystemWidget::_processFiles( const File::List& files )
{

    // check path
    if( thread_->file() != pathEditor_->path() )
    {
        _update();
        return;
    }

    // build records
    FileRecord::List records;

    // add navigator if needed and update model
    if( showNavigator_ )
    {
        FileRecord record( File("..") );
        record.setFlags( BaseFileInfo::Navigator );
        records.append( record );
    }

    // add files
    for( const auto& file:files )
    {
        // skip hidden files
        if( file.isHidden() && !hiddenFilesAction_->isChecked() ) continue;

        // create file record
        FileRecord record( file, file.lastModified() );

        // assign size
        record.addProperty( sizePropertyId_, QString::number(file.fileSize()) );

        // assign type
        record.setFlag( file.isDirectory() ? BaseFileInfo::Folder : BaseFileInfo::Document );
        if( file.isLink() ) record.setFlag( BaseFileInfo::Link );
        if( file.isHidden() ) record.setFlag( BaseFileInfo::Hidden );

        // add to model
        records.append( record );

    }

    // update model
    model_.update( records );

    // update list
    list_->updateMask();
    list_->resizeColumnToContents( FileSystemModel::FileName );

    unsetCursor();

}

//______________________________________________________
void BaseFileSystemWidget::_itemActivated( const QModelIndex& index )
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_itemActivated.\n") );

    if( !index.isValid() ) return;

    // retrieve file
    FileRecord record( model_.get( index ) );
    if( record.hasFlag( BaseFileInfo::Folder ) )
    {

        setPath( File( record.file() ).addPath( pathEditor_->path() ) );

    } else if( record.hasFlag( BaseFileInfo::Navigator ) ) {

        pathEditor_->selectParent();

    } else {

        emit fileActivated( record.setFile( File( record.file() ).addPath( pathEditor_->path() ) ) );

    }

}

//______________________________________________________
void BaseFileSystemWidget::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_updateConfiguration.\n") );
    hiddenFilesAction_->setChecked( XmlOptions::get().get<bool>( QStringLiteral("SHOW_HIDDEN_FILES") ) );

    // show navigator in list
    showNavigator_ = XmlOptions::get().get<bool>( QStringLiteral("SHOW_NAVIGATOR") );

}

//______________________________________________________
void BaseFileSystemWidget::_updateNavigationActions()
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_updateNavigationActions.\n") );
    previousDirectoryAction_->setEnabled( pathEditor_->hasPrevious() );
    nextDirectoryAction_->setEnabled( pathEditor_->hasNext() );
    parentDirectoryAction_->setEnabled( pathEditor_->hasParent() );
    return;
}

//______________________________________________________
void BaseFileSystemWidget::_updateFileSystemWatcher()
{
    const File path( pathEditor_->path() );
    if( path.isEmpty() || !( path.exists() && path.isDirectory() ) ) return;

    // reset file system watcher
    const auto directories( fileSystemWatcher_.directories() );
    if( !directories.isEmpty() ) fileSystemWatcher_.removePaths( directories );

    fileSystemWatcher_.addPath( path );
}

//______________________________________________________
void BaseFileSystemWidget::_update( const QString& value )
{ if( isVisible() && value == pathEditor_->path() ) _update(); }

//______________________________________________________
void BaseFileSystemWidget::_update()
{

    const File path( pathEditor_->path() );
    if( path.isEmpty() || !( path.exists() && path.isDirectory() ) ) return;
    if( thread_->isRunning() ) thread_->wait();

    // setup thread
    thread_->setFile( path );
    thread_->setCommand( FileThread::Command::List );
    thread_->setFlags(  hiddenFilesAction_->isChecked() ? File::ListFlag::ShowHiddenFiles : File::ListFlag::None );
    setCursor( Qt::WaitCursor );

    thread_->start();

}

//______________________________________________________________________
void BaseFileSystemWidget::_updateActions()
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget:_updateActions.\n") );
    FileRecord::List selection( model_.get( list_->selectionModel()->selectedRows() ) );

    const bool hasEditableSelection = std::any_of( selection.begin(), selection.end(), FileRecord::HasFlagFTor( BaseFileInfo::Document ) );
    const bool hasRemovableSelection = hasEditableSelection || !std::all_of(  selection.begin(), selection.end(), FileRecord::HasFlagFTor( BaseFileInfo::Navigator ) );

    openAction_->setEnabled( hasEditableSelection );
    removeAction_->setEnabled( hasRemovableSelection );

    QModelIndex index( list_->selectionModel()->currentIndex() );
    bool hasValidFile( selection.size() == 1 && index.isValid() && !model_.get( index ).hasFlag( BaseFileInfo::Navigator ) );
    filePropertiesAction_->setEnabled( hasValidFile );
    renameAction_->setEnabled( hasValidFile );
}

//______________________________________________________
void BaseFileSystemWidget::_showToolTip( const QModelIndex& index )
{

    if( !index.isValid() ) toolTipWidget_->hide();
    else {

        // update tooltip content
        const auto& record( model_.get( index ) );
        QIcon icon;
        const auto iconVariant( model_.data( index, Qt::DecorationRole ) );
        if( iconVariant.canConvert( QVariant::Icon ) ) icon = iconVariant.value<QIcon>();
        toolTipWidget_->setRecord( record, icon );

        // move tooltip
        toolTipWidget_->setIndexRect(
            list_->visualRect( index ).
            translated( list_->viewport()->mapToGlobal( QPoint( 0, 0 ) ) ) );

        // show
        toolTipWidget_->showDelayed();

    }

}

//______________________________________________________
void BaseFileSystemWidget::_toggleShowHiddenFiles( bool state )
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_toggleShowHiddenFiles.\n") );
    XmlOptions::get().set( QStringLiteral("SHOW_HIDDEN_FILES"), state );
}

//______________________________________________________
void BaseFileSystemWidget::_reload()
{
    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_reload.\n") );
    setPath( pathEditor_->path(), true );
}

//______________________________________________________________________
void BaseFileSystemWidget::_open()
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget:_open.\n") );
    const auto selection( model_.get( list_->selectionModel()->selectedRows() ) );
    for( const auto& record:selection )
    {
        if( record.hasFlag( BaseFileInfo::Document ) )
        {
            FileRecord copy( record );
            copy.setFile( File( record.file() ).addPath( pathEditor_->path() ) );
            emit fileActivated( copy );
        }

    }

}

//______________________________________________________________________
void BaseFileSystemWidget::_remove()
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_remove.\n") );

    // get selection
    FileSystemModel::List selection( model_.get( list_->selectionModel()->selectedRows() ) );
    FileSystemModel::List validSelection;
    for( const auto& record:selection )
    {
        if( record.hasFlag( BaseFileInfo::Navigator ) ) continue;
        FileRecord copy( record );
        copy.setFile( File( record.file() ).addPath( pathEditor_->path() ) );
        validSelection.append( copy );

    }

    if( validSelection.empty() ) return;
    RemoveFilesDialog dialog( this, validSelection );
    dialog.setWindowTitle( tr( "Delete Files" ) );
    if( !dialog.exec() ) return;

    // loop over selected files and remove
    // retrieve selected items and remove corresponding files
    for( const auto& record:validSelection )
    { if( record.file().exists() ) record.file().removeRecursive(); }


}

//________________________________________
void BaseFileSystemWidget::_rename()
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_rename.\n") );
    const auto index( list_->selectionModel()->currentIndex() );
    if( !index.isValid() ) return;

    const auto record( model_.get( index ) );
    if( record.hasFlag( BaseFileInfo::Navigator ) ) return;
    RenameFileDialog dialog( this, record );
    dialog.setWindowTitle( tr( "Rename Item" ) );
    dialog.centerOnWidget( window() );
    if( !dialog.exec() ) return;

    File newFile( dialog.file() );
    if( newFile == record.file() ) return;

    // rename
    File( record.file() ).addPath( pathEditor_->path() ).rename( newFile.addPath( pathEditor_->path() ) );

}

//________________________________________
void BaseFileSystemWidget::_fileProperties()
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_fileProperties.\n") );
    const auto index( list_->selectionModel()->currentIndex() );
    if( !index.isValid() ) return;

    FileRecord record( model_.get( index ) );
    if( record.hasFlag( BaseFileInfo::Navigator ) ) return;
    if( !record.file().isAbsolute() ) { record.setFile( File( record.file() ).addPath( pathEditor_->path() ) ); }

    // icon
    QIcon icon;
    const auto iconVariant( model_.data( index, Qt::DecorationRole ) );
    if( iconVariant.canConvert( QVariant::Icon ) ) icon = iconVariant.value<QIcon>();

    FileInformationDialog dialog( this, record );
    dialog.setIcon( icon );
    dialog.centerOnWidget( window() );
    dialog.exec();

}

//_____________________________________________
void BaseFileSystemWidget::_installActions()
{

    Debug::Throw( QStringLiteral("BaseFileSystemWidget::_installActions.\n") );

    // hidden files
    addAction( hiddenFilesAction_ = new QAction( tr( "Show Hidden Files" ), this ) );
    hiddenFilesAction_->setCheckable( true );
    connect( hiddenFilesAction_, &QAction::toggled, this, [this](bool) { _update(); } );
    connect( hiddenFilesAction_, &QAction::toggled, this, &BaseFileSystemWidget::_toggleShowHiddenFiles );

    // previous directory (from history)
    addAction( previousDirectoryAction_ = new QAction( IconEngine::get( IconNames::PreviousDirectory ), tr( "Back" ), this ) );
    connect( previousDirectoryAction_, &QAction::triggered, pathEditor_, &PathEditor::selectPrevious );

    // next directory (from history)
    addAction( nextDirectoryAction_ = new QAction( IconEngine::get( IconNames::NextDirectory ), tr( "Forward" ), this ) );
    connect( nextDirectoryAction_, &QAction::triggered, pathEditor_, &PathEditor::selectNext );

    // parent directory in tree
    addAction( parentDirectoryAction_ = new QAction( IconEngine::get( IconNames::Parent ), tr( "Parent Directory" ), this ) );
    parentDirectoryAction_->setIconText( tr( "Up" ) );
    connect( parentDirectoryAction_, &QAction::triggered, pathEditor_, &PathEditor::selectParent );
    parentDirectoryAction_->setToolTip( tr( "Change path to parent directory" ) );

    // home directory
    addAction( homeDirectoryAction_ = new QAction( IconEngine::get( IconNames::Home ), tr( "Home" ), this ) );
    connect( homeDirectoryAction_, &QAction::triggered, this, &BaseFileSystemWidget::_homeDirectory );
    homeDirectoryAction_->setToolTip( tr( "Change path to current user home directory" ) );

    // working directory
    addAction( workingDirectoryAction_ = new QAction( IconEngine::get( IconNames::Find ), tr( "Working Directory" ), this ) );
    connect( workingDirectoryAction_, &QAction::triggered, this, &BaseFileSystemWidget::_workingDirectory );
    workingDirectoryAction_->setToolTip( tr( "Change path to current file working directory" ) );

    // reload
    addAction( reloadAction_ = new QAction( IconEngine::get( IconNames::Reload ), tr( "Reload" ), this ) );
    connect( reloadAction_, &QAction::triggered, this, &BaseFileSystemWidget::_reload );
    reloadAction_->setToolTip( tr( "Reload current directory contents" ) );

    // open
    addAction( openAction_ = new QAction( IconEngine::get( IconNames::Open ), tr( "Open Selected Files" ), this ) );
    connect( openAction_, &QAction::triggered, this, &BaseFileSystemWidget::_open );
    openAction_->setToolTip( tr( "Edit selected files" ) );

    // remove
    addAction( removeAction_ = new QAction( IconEngine::get( IconNames::Delete ), tr( "Delete" ), this ) );
    connect( removeAction_, &QAction::triggered, this, &BaseFileSystemWidget::_remove );
    removeAction_->setShortcut( QKeySequence::Delete );
    removeAction_->setShortcutContext( Qt::WidgetWithChildrenShortcut );

    // rename
    addAction( renameAction_ = new QAction( IconEngine::get( IconNames::Rename ), tr( "Rename" ), this ) );
    connect( renameAction_, &QAction::triggered, this, &BaseFileSystemWidget::_rename );
    renameAction_->setShortcut( Qt::Key_F2 );
    renameAction_->setToolTip( tr( "Change selected file name" ) );

    // file properties
    addAction( filePropertiesAction_ = new QAction( IconEngine::get( IconNames::Information ), tr( "Properties..." ), this ) );
    connect( filePropertiesAction_, &QAction::triggered, this, &BaseFileSystemWidget::_fileProperties );
    filePropertiesAction_->setToolTip( tr( "Display current file properties" ) );

}
