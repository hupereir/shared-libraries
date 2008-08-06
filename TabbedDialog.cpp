
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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/
  
/*!
  \file TabbedDialog.cpp
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QApplication>
#include <QScrollArea>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>

#include "TabbedDialog.h"

using namespace std;

//_________________________________________________________
TabbedDialog::TabbedDialog( QWidget* parent ):
  QDialog( parent ),
  Counter( "TabbedDialog" )
{
  
  
  Debug::Throw( "TabbedDialog::TabbedDialog.\n" );
  setSizeGripEnabled( true );
  
  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setSpacing(10);
  layout->setMargin(10);
  setLayout( layout );
  
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setMargin(0);
  h_layout->setSpacing(10);
  layout->addLayout( h_layout );
  
  h_layout->addWidget( list_ = new TreeWidget( this ), 0 );
  h_layout->addWidget( stack_ = new QStackedWidget(0), 1 );
  
  _list().setColumnCount(1);
  _list().setMaximumWidth(150);
  _list().header()->hide();
  _list().setSortingEnabled( false );

  connect( 
    &_list(), SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), 
    SLOT( _display(QTreeWidgetItem*, QTreeWidgetItem*) ) );
  
  // button layout
  button_layout_ = new QHBoxLayout();
  button_layout_->setMargin(0);
  button_layout_->setSpacing(5);
  layout->addLayout( button_layout_, 0 );
  
  // close window shortcut
  connect( new QShortcut( Qt::CTRL+Qt::Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );
  connect( this, SIGNAL( accepted() ), SLOT( _saveWindowSize() ) );
  connect( this, SIGNAL( rejected() ), SLOT( _saveWindowSize() ) );
  
}

//_________________________________________________________
QWidget& TabbedDialog::addPage( const QString& title, const bool& expand )
{  
  Debug::Throw( "ConfigList::Item::Item.\n" );
  
  
  QScrollArea* scroll = new QScrollArea();
  scroll->setWidgetResizable ( true );
  scroll->setFrameStyle( QFrame::NoFrame );
  
  QWidget* main( new QWidget() );
  scroll->setWidget( main );
  _stack().addWidget( scroll );
  
  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setSpacing( 5 );
  layout->setMargin( 0 );
  main->setLayout( layout );
  
  // create new item and add to stack
  new ConfigListItem( &_list(), title, scroll );
  
  // make sure item is selected if first in list
  if( _list().topLevelItemCount() == 1 )
  { _list().selectionModel()->select( _list().model()->index(0,0), QItemSelectionModel::Select|QItemSelectionModel::Rows ); }
  
  // in expanded mode, the main widget is returned directly
  if( expand ) return *main;
  
  // in non-expanded mode (the default)
  // a widget is created inside main, and a stretch is added at the bottom
  // the created widget is return
  QWidget* contents( new QWidget( main ) );
  contents->setLayout( new QVBoxLayout() );
  contents->layout()->setSpacing(5);
  contents->layout()->setMargin(0);
  
  layout->addWidget( contents );
  layout->addStretch();
  return *contents;
  
}

//__________________________________________________
void TabbedDialog::_display( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
  Debug::Throw( "TabbedDialog::_display.\n" );
  
  if( !current ) current = previous;
  ConfigListItem* item( dynamic_cast<ConfigListItem*>(current) );
  assert( item );
  _stack().setCurrentWidget(&item->page());  
}


//__________________________________________________
QSize TabbedDialog::minimumSizeHint( void ) const
{
  
  Debug::Throw() << "TabbedDialog::minimumSizeHint - _sizeOptionName: " << _sizeOptionName() << endl;
  
  // resize
  if( !_sizeOptionName().empty() && XmlOptions::get().find( _sizeOptionName()+"_WIDTH" ) && XmlOptions::get().find( _sizeOptionName()+"_HEIGHT" ) )
  {
    
    int width( XmlOptions::get().get<int>( _sizeOptionName()+"_WIDTH" ) );
    int height( XmlOptions::get().get<int>( _sizeOptionName()+"_HEIGHT" ) );
    Debug::Throw() << "TabbedDialog::minimumSizeHint: " << width << "x" << height << endl;
    return QSize( width, height );

  } else return QDialog::minimumSizeHint();
 
}


//__________________________________________________
QSize TabbedDialog::sizeHint( void ) const
{
  
  Debug::Throw() << "TabbedDialog::sizeHint - _sizeOptionName: " << _sizeOptionName()+"_WIDTH" << " found: " << XmlOptions::get().find( _sizeOptionName()+"_WIDTH" ) << endl;
  
  // resize
  if( XmlOptions::get().find( _sizeOptionName()+"_WIDTH" ) && XmlOptions::get().find( _sizeOptionName()+"_HEIGHT" ) )
  {
    
    int width( XmlOptions::get().get<int>( _sizeOptionName()+"_WIDTH" ) );
    int height( XmlOptions::get().get<int>( _sizeOptionName()+"_HEIGHT" ) );
    Debug::Throw() << "TabbedDialog::sizeHint: " << width << "x" << height << endl;
    return QSize( width, height );

  } else return QDialog::sizeHint();
 
}

//__________________________________________________
void TabbedDialog::closeEvent( QCloseEvent* e )
{
  
  _saveWindowSize();
  return QDialog::closeEvent( e );
  
}

//__________________________________________________
void TabbedDialog::_saveWindowSize( void )
{
  Debug::Throw() << "TabbedDialog::_saveWindowSize: " << width() << "x" << height() << endl;
  if( !_sizeOptionName().empty() )
  {
    XmlOptions::get().set<int>( _sizeOptionName()+"_WIDTH", width() );
    XmlOptions::get().set<int>( _sizeOptionName()+"_HEIGHT", height() );
  }
}
