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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
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

#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QShortcut>

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
    layout->addWidget( box = new QGroupBox( "Options", this ) );

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
    hLayout->addWidget( new QLabel( "Recent files history size", box ) );
    hLayout->addWidget( spinbox = new OptionSpinBox( box, "DB_SIZE" ) );
    hLayout->addStretch(1);
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 100 );
    addOptionWidget( spinbox );
    spinbox->setToolTip( "Number of previously opened files to appear in the Open Previous menu" );

    // sort previous files by date
    OptionCheckBox* checkbox;
    vLayout->addWidget( checkbox = new OptionCheckBox( "Sort recent files by date", box, "SORT_FILES_BY_DATE" ) );
    checkbox->setToolTip( "Sort files by date rather than name in Open Previous menu." );
    addOptionWidget( checkbox );

    // group box for file list
    layout->addWidget( box = new QGroupBox( "Files", this ), 1 );
    box->setLayout( new QHBoxLayout() );
    box->layout()->setSpacing(5);
    box->layout()->setMargin(5);

    box->layout()->addWidget( list_ = new TreeView( box ) );
    _list().setModel( &model_ );
    _list().sortByColumn( XmlOptions::get().get<bool>( "SORT_FILES_BY_DATE" ) ? FileRecordModel::TIME:FileRecordModel::FILE );
    _list().setSelectionMode( QAbstractItemView::ContiguousSelection );
    _list().setOptionName( "RECENT_FILES_CONFIGURATION_LIST" );

    connect( _list().selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ), SLOT( _updateButtons() ) );

    vLayout = new QVBoxLayout();
    vLayout->setSpacing(5);
    vLayout->setMargin(0);
    box->layout()->addItem( vLayout );

    // list context menu
    QMenu* menu( new ContextMenu( &_list() ) );

    // clean
    vLayout->addWidget( cleanButton_ = new QPushButton( "Clean", box ) );
    cleanButton_->setToolTip( "Remove invalid files" );
    cleanButton_->setIcon( IconEngine::get( ICONS::DELETE ) );
    connect( cleanButton_, SIGNAL( clicked() ), SLOT( _clean() ) );

    addAction( cleanAction_ = new QAction( IconEngine::get( ICONS::DELETE ), "Clean", this ) );
    connect( cleanAction_, SIGNAL( triggered() ), SLOT( _clean() ) );
    menu->addAction( cleanAction_ );

    // remove
    vLayout->addWidget( removeButton_ = new QPushButton( "Remove", box ) );
    removeButton_->setIcon( IconEngine::get( ICONS::REMOVE ) );
    removeButton_->setToolTip( "Remove selected files" );
    connect( removeButton_, SIGNAL( clicked() ), SLOT( _remove() ) );

    addAction( removeAction_ = new QAction( IconEngine::get( ICONS::REMOVE ), "Remove", this ) );
    connect( removeAction_, SIGNAL( triggered() ), SLOT( _remove() ) );
    removeAction_->setShortcut( QKeySequence::Delete );
    menu->addAction( removeAction_ );
    _list().addAction( removeAction_ );

    // reload
    vLayout->addWidget( reloadButton_ = new QPushButton( "Reload", box ) );
    reloadButton_->setToolTip( "Reload file list" );
    reloadButton_->setIcon( IconEngine::get( ICONS::RELOAD ) );
    connect( reloadButton_, SIGNAL( clicked() ), SLOT( _reload() ) );

    addAction( reloadAction_ = new QAction( IconEngine::get( ICONS::RELOAD ), "Reload", this ) );
    connect( reloadAction_, SIGNAL( triggered() ), SLOT( _reload() ) );
    menu->addAction( reloadAction_ );

    vLayout->addStretch( 1 );

    // connections
    _reloadButton().setEnabled( false );

}

//__________________________________________________________________________
void RecentFilesConfiguration::read( void )
{
    Debug::Throw( "RecentFilesConfiguration::read.\n" );
    OptionWidgetList::read();

    _reload();
    _list().resizeColumns();

}

//__________________________________________________________________________
void RecentFilesConfiguration::write( void ) const
{
    Debug::Throw( "RecentFilesConfiguration::write.\n" );
    OptionWidgetList::write();

    // put model contents into file list
    FileRecord::List records;
    foreach( const FileRecord& record, _model().get() )
    { records << record; }

    _recentFiles().set( records );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_updateButtons( void )
{

    Debug::Throw( "RecentFilesConfiguration::_updateButtons.\n" );
    QList<QModelIndex> selection( _list().selectionModel()->selectedRows() );
    _removeButton().setEnabled( !selection.empty() );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_remove( void )
{

    Debug::Throw( "RecentFilesConfiguration::_remove.\n" );
    QList<QModelIndex> selection( _list().selectionModel()->selectedRows() );
    if( selection.isEmpty() ) return;

    // ask confirmation
    if( !QuestionDialog( this, "Remove selected files from list ?" ).centerOnParent().exec() ) return;

    _model().remove( _model().get( selection ) );
    _list().selectionModel()->clear();

    _reloadButton().setEnabled( true );

    // records
    FileRecordModel::List records( _model().get() );
    _cleanButton().setEnabled( std::find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) != records.end() );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_clean( void )
{
    Debug::Throw( "RecentFilesConfiguration::_clean.\n" );

    // remove invalid files
    FileRecordModel::List records( _model().get() );
    records.erase(
        std::remove_if( records.begin(), records.end(), FileRecord::InvalidFTor() ),
        records.end() );

    _model().set( records );

    _reloadButton().setEnabled( true );
    _cleanButton().setEnabled( false );
    _updateButtons();

}

//__________________________________________________________________________
void RecentFilesConfiguration::_reload( void )
{

    Debug::Throw( "RecentFilesConfiguration::_reload.\n" );
    FileRecordModel::List recordModelList;
    foreach( const FileRecord& record, _recentFiles().records() )
    { recordModelList << record; }
    _model().set( recordModelList );

    _list().selectionModel()->clear();

    _reloadButton().setEnabled( false );
    _cleanButton().setEnabled( std::find_if( recordModelList.begin(), recordModelList.end(), FileRecord::InvalidFTor() ) != recordModelList.end() );
    _updateButtons();

}
