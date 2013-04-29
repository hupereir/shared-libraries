// $Id$

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

#include "RecentFilesConfiguration.h"
#include "BaseIcons.h"
#include "ContextMenu.h"
#include "Debug.h"
#include "FileList.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "OptionCheckBox.h"
#include "OptionSpinBox.h"
#include "QuestionDialog.h"
#include "TreeView.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QShortcut>

//___________________________________________
RecentFilesConfiguration::RecentFilesConfiguration( QWidget* parent, FileList& recent_files ):
    QWidget( parent ),
    Counter( "RecentFilesConfiguration" ),
    recentFiles_( &recent_files )
{
    Debug::Throw( "RecentFilesConfiguration::RecentFilesConfiguration.\n" );

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );
    layout->setMargin(0);
    layout->setSpacing( 5 );

    QGroupBox* box;
    // group box for additional options
    layout->addWidget( box = new QGroupBox( tr( "Options" ), this ) );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(5);
    vLayout->setSpacing(5);
    box->setLayout( vLayout );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(5);
    hLayout->setMargin(0);
    vLayout->addLayout( hLayout );

    // previous file history size
    OptionSpinBox *spinbox;
    hLayout->addWidget( new QLabel( tr( "Recent files history size" ), box ) );
    hLayout->addWidget( spinbox = new OptionSpinBox( box, "DB_SIZE" ) );
    hLayout->addStretch(1);
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 100 );
    addOptionWidget( spinbox );
    spinbox->setToolTip( tr( "Number of previously opened files to appear in the Open Previous menu" ) );

    // sort previous files by date
    OptionCheckBox* checkbox;
    vLayout->addWidget( checkbox = new OptionCheckBox( tr( "Sort recent files by date" ), box, "SORT_FILES_BY_DATE" ) );
    checkbox->setToolTip( tr( "Sort files by date rather than name in Open Previous menu." ) );
    addOptionWidget( checkbox );

    // group box for file list
    layout->addWidget( box = new QGroupBox( tr( "Files" ), this ), 1 );
    box->setLayout( new QHBoxLayout() );
    box->layout()->setSpacing(5);
    box->layout()->setMargin(5);

    box->layout()->addWidget( list_ = new TreeView( box ) );
    list_->setModel( &model_ );
    list_->sortByColumn( XmlOptions::get().get<bool>( "SORT_FILES_BY_DATE" ) ? FileRecordModel::TIME:FileRecordModel::FILE );
    list_->setSelectionMode( QAbstractItemView::ContiguousSelection );
    list_->setOptionName( "RECENT_FILES_CONFIGURATION_LIST" );

    connect( list_->selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ), SLOT( _updateButtons() ) );

    vLayout = new QVBoxLayout();
    vLayout->setSpacing(5);
    vLayout->setMargin(0);
    box->layout()->addItem( vLayout );

    // list context menu
    QMenu* menu( new ContextMenu( list_ ) );

    // clean
    vLayout->addWidget( cleanButton_ = new QPushButton( tr( "Clean" ), box ) );
    cleanButton_->setToolTip( tr( "Remove invalid files" ) );
    cleanButton_->setIcon( IconEngine::get( ICONS::DELETE ) );
    connect( cleanButton_, SIGNAL( clicked() ), SLOT( _clean() ) );

    addAction( cleanAction_ = new QAction( IconEngine::get( ICONS::DELETE ), tr( "Clean" ), this ) );
    connect( cleanAction_, SIGNAL( triggered() ), SLOT( _clean() ) );
    menu->addAction( cleanAction_ );

    // remove
    vLayout->addWidget( removeButton_ = new QPushButton( tr( "Remove" ), box ) );
    removeButton_->setIcon( IconEngine::get( ICONS::REMOVE ) );
    removeButton_->setToolTip( tr( "Remove selected files" ) );
    connect( removeButton_, SIGNAL( clicked() ), SLOT( _remove() ) );

    addAction( removeAction_ = new QAction( IconEngine::get( ICONS::REMOVE ), tr( "Remove" ), this ) );
    connect( removeAction_, SIGNAL( triggered() ), SLOT( _remove() ) );
    removeAction_->setShortcut( QKeySequence::Delete );
    menu->addAction( removeAction_ );
    list_->addAction( removeAction_ );

    // reload
    vLayout->addWidget( reloadButton_ = new QPushButton( tr( "Reload" ), box ) );
    reloadButton_->setToolTip( tr( "Reload file list" ) );
    reloadButton_->setIcon( IconEngine::get( ICONS::RELOAD ) );
    connect( reloadButton_, SIGNAL( clicked() ), SLOT( _reload() ) );

    addAction( reloadAction_ = new QAction( IconEngine::get( ICONS::RELOAD ), tr( "Reload" ), this ) );
    connect( reloadAction_, SIGNAL( triggered() ), SLOT( _reload() ) );
    menu->addAction( reloadAction_ );

    vLayout->addStretch( 1 );

    // connections
    reloadButton_->setEnabled( false );

}

//__________________________________________________________________________
void RecentFilesConfiguration::read( void )
{
    Debug::Throw( "RecentFilesConfiguration::read.\n" );
    OptionWidgetList::read();

    _reload();
    list_->resizeColumns();

}

//__________________________________________________________________________
void RecentFilesConfiguration::write( void ) const
{
    Debug::Throw( "RecentFilesConfiguration::write.\n" );
    OptionWidgetList::write();

    // put model contents into file list
    FileRecord::List records;
    foreach( const FileRecord& record, model_.get() )
    { records << record; }

    recentFiles_->set( records );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_updateButtons( void )
{

    Debug::Throw( "RecentFilesConfiguration::_updateButtons.\n" );
    QList<QModelIndex> selection( list_->selectionModel()->selectedRows() );
    removeButton_->setEnabled( !selection.empty() );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_remove( void )
{

    Debug::Throw( "RecentFilesConfiguration::_remove.\n" );
    QList<QModelIndex> selection( list_->selectionModel()->selectedRows() );
    if( selection.isEmpty() ) return;

    // ask confirmation
    if( !QuestionDialog( this, tr( "Remove selected files from list ?" ) ).centerOnParent().exec() ) return;

    model_.remove( model_.get( selection ) );
    list_->selectionModel()->clear();

    reloadButton_->setEnabled( true );

    // records
    FileRecordModel::List records( model_.get() );
    cleanButton_->setEnabled( std::find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) != records.end() );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_clean( void )
{
    Debug::Throw( "RecentFilesConfiguration::_clean.\n" );

    // remove invalid files
    FileRecordModel::List records( model_.get() );
    records.erase(
        std::remove_if( records.begin(), records.end(), FileRecord::InvalidFTor() ),
        records.end() );

    model_.set( records );

    reloadButton_->setEnabled( true );
    cleanButton_->setEnabled( false );
    _updateButtons();

}

//__________________________________________________________________________
void RecentFilesConfiguration::_reload( void )
{

    Debug::Throw( "RecentFilesConfiguration::_reload.\n" );
    FileRecordModel::List recordModelList;
    foreach( const FileRecord& record, recentFiles_->records() )
    { recordModelList << record; }
    model_.set( recordModelList );

    list_->selectionModel()->clear();

    reloadButton_->setEnabled( false );
    cleanButton_->setEnabled( std::find_if( recordModelList.begin(), recordModelList.end(), FileRecord::InvalidFTor() ) != recordModelList.end() );
    _updateButtons();

}
