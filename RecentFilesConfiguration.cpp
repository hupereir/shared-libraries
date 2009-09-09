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


/*!
   \file RecentFilesConfiguration.cpp
   \brief   Spell checker common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QShortcut>

#include "BaseIcons.h"
#include "Debug.h"
#include "FileList.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "OptionCheckBox.h"
#include "OptionSpinBox.h"
#include "QuestionDialog.h"
#include "RecentFilesConfiguration.h"
#include "TreeView.h"

using namespace std;

//___________________________________________
RecentFilesConfiguration::RecentFilesConfiguration( QWidget* parent, FileList& recent_files ):
  QWidget( parent ),
  Counter( "RecentFilesConfiguration" ),
  recent_files_( &recent_files )
{
  Debug::Throw( "RecentFilesConfiguration::RecentFilesConfiguration.\n" );

  QVBoxLayout* layout = new QVBoxLayout();
  setLayout( layout );
  layout->setMargin(0);
  layout->setSpacing( 5 );

  QGroupBox* box;
  // group box for additional options
  layout->addWidget( box = new QGroupBox( "Options", this ) );

  QVBoxLayout* v_layout = new QVBoxLayout();
  v_layout->setMargin(5);
  v_layout->setSpacing(5);
  box->setLayout( v_layout );

  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setSpacing(5);
  h_layout->setMargin(0);
  v_layout->addLayout( h_layout );


  // previous file history size
  OptionSpinBox *spinbox;
  h_layout->addWidget( new QLabel( "Recent files history size", box ) );
  h_layout->addWidget( spinbox = new OptionSpinBox( box, "DB_SIZE" ) );
  h_layout->addStretch(1);
  spinbox->setMinimum( 0 );
  spinbox->setMaximum( 100 );
  addOptionWidget( spinbox );
  spinbox->setToolTip( "Number of previously opened files to appear in the Open Previous menu" );

  // sort previous files by date
  OptionCheckBox* checkbox;
  v_layout->addWidget( checkbox = new OptionCheckBox( "Sort recent files by date", box, "SORT_FILES_BY_DATE" ) );
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

  connect( _list().selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ), SLOT( _updateButtons() ) );

  v_layout = new QVBoxLayout();
  v_layout->setSpacing(5);
  v_layout->setMargin(0);
  box->layout()->addItem( v_layout );

  // clean
  v_layout->addWidget( clean_button_ = new QPushButton( "&Clean", box ) );
  clean_button_->setToolTip( "Remove invalid files" );
  clean_button_->setIcon( IconEngine::get( ICONS::DELETE ) );
  connect( clean_button_, SIGNAL( clicked() ), SLOT( _clean() ) );

  addAction( clean_action_ = new QAction( IconEngine::get( ICONS::DELETE ), "&Clean", this ) );
  connect( clean_action_, SIGNAL( triggered() ), SLOT( _clean() ) );
  _list().menu().addAction( clean_action_ );

  // remove
  v_layout->addWidget( remove_button_ = new QPushButton( "&Remove", box ) );
  remove_button_->setIcon( IconEngine::get( ICONS::REMOVE ) );
  remove_button_->setToolTip( "Remove selected files" );
  connect( remove_button_, SIGNAL( clicked() ), SLOT( _remove() ) );

  addAction( remove_action_ = new QAction( IconEngine::get( ICONS::REMOVE ), "&Remove", this ) );
  connect( remove_action_, SIGNAL( triggered() ), SLOT( _remove() ) );
  remove_action_->setShortcut( Qt::Key_Delete );
  _list().menu().addAction( remove_action_ );
  _list().addAction( remove_action_ );

  // reload
  v_layout->addWidget( reload_button_ = new QPushButton( "&Reload", box ) );
  reload_button_->setToolTip( "Reload file list" );
  reload_button_->setIcon( IconEngine::get( ICONS::RELOAD ) );
  connect( reload_button_, SIGNAL( clicked() ), SLOT( _reload() ) );

  addAction( reload_action_ = new QAction( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
  connect( reload_action_, SIGNAL( triggered() ), SLOT( _reload() ) );
  _list().menu().addAction( reload_action_ );

  v_layout->addStretch( 1 );

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
  _recentFiles().set( _model().get() );

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
  _cleanButton().setEnabled( find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) != records.end() );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_clean( void )
{
  Debug::Throw( "RecentFilesConfiguration::_clean.\n" );

  // remove invalid files
  FileRecordModel::List records( _model().get() );
  records.erase(
    remove_if( records.begin(), records.end(), FileRecord::InvalidFTor() ),
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
  FileRecord::List records( _recentFiles().records() );
  _model().set( records );
  _list().selectionModel()->clear();

  _reloadButton().setEnabled( false );
  _cleanButton().setEnabled( find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) != records.end() );
  _updateButtons();

}
