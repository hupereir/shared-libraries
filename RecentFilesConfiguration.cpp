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

#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QLabel>

#include "Debug.h"
#include "FileList.h"
#include "GridLayout.h"
#include "OptionCheckBox.h"
#include "OptionSpinBox.h"
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
  
  GridLayout* grid_layout = new GridLayout();
  grid_layout->setSpacing(5);
  grid_layout->setMargin(5);
  grid_layout->setMaxCount(2);
  box->setLayout( grid_layout );

  // previous file history size
  OptionSpinBox *spinbox;
  grid_layout->addWidget( new QLabel( "Recent files history size", box ) );
  grid_layout->addWidget( spinbox = new OptionSpinBox( box, "DB_SIZE" ) );
  spinbox->setMinimum( 0 );
  spinbox->setMaximum( 100 );
  addOptionWidget( spinbox );
  spinbox->setToolTip( "Number of previously opened files to appear in the Open Previous menu" );

  // sort previous files by date
  OptionCheckBox* checkbox;
  grid_layout->addWidget( checkbox = new OptionCheckBox( "Sort recent files by date", box, "SORT_FILES_BY_DATE" ), 3, 0, 1, 2 );
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

  QVBoxLayout* v_layout = new QVBoxLayout();
  v_layout->setSpacing(5);
  v_layout->setMargin(0);
  box->layout()->addItem( v_layout );
  
  QPushButton* button;
  v_layout->addWidget( button = new QPushButton( "&Remove", box ) );
  button->setToolTip( "Remove selected files" );
  remove_button_ = button;
  
  v_layout->addWidget( button = new QPushButton( "&Clear", box ) );
  button->setToolTip( "Remove all files" );
  clear_button_ = button;
  
  v_layout->addWidget( button = new QPushButton( "&Reload", box ) );
  button->setToolTip( "Reload file list" );
  reload_button_ = button;
  
  v_layout->addStretch( 1 );
 
  // connections
  connect( _list().selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ), SLOT( _updateButtons() ) );
  connect( remove_button_, SIGNAL( clicked() ), SLOT( _remove() ) );
  connect( clear_button_, SIGNAL( clicked() ), SLOT( _clear() ) );
  connect( reload_button_, SIGNAL( clicked() ), SLOT( _reload() ) );
  
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
  
  _model().remove( _model().get( selection ) );
  _list().selectionModel()->clear();
  
  _reloadButton().setEnabled( true );
  _clearButton().setEnabled( !_model().get().empty() );
  
}

//__________________________________________________________________________
void RecentFilesConfiguration::_clear( void )
{
  Debug::Throw( "RecentFilesConfiguration::_clear.\n" );
  _model().clear();
  _reloadButton().setEnabled( true );
  _clearButton().setEnabled( false );
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
  _clearButton().setEnabled( !records.empty() );
  _updateButtons();

}
