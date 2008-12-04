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
  \file ListWidget.cpp
  \brief qlistbox with even objects highlighted with different color
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "ColorDisplay.h"
#include "ListWidget.h"
#include "Singleton.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
ListWidget::ListWidget( QWidget* parent ):
  QListWidget( parent ),
  Counter( "ListWidget" )
{
  
  Debug::Throw( "ListWidget::ListWidget.\n" ); 
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
  
}  

//_______________________________________________
QListWidgetItem& ListWidget::findItem( const std::string& name )
{
  QList<QListWidgetItem*> items( findItems( name.c_str(), Qt::MatchExactly ) );
  return ( !items.empty() ) ? *items.front() : *(new QListWidgetItem( name.c_str(), this, 0 ) );  
}


//_____________________________________________________________________
void ListWidget::_updateConfiguration( void )
{
  
  Debug::Throw( "ListWidget::_updateConfiguration.\n" );
  QColor item_color;
  
  // try load from option
  Str colorname( XmlOptions::get().get<string>("ALTERNATE_COLOR").c_str() );
  if( !colorname.isEqual( qPrintable( ColorDisplay::NONE ), false ) ) item_color = QColor( colorname.c_str() );

  if( !item_color.isValid() )
  {
    setAlternatingRowColors( false ); 
    return;
  }
  
  QPalette palette( this->palette() );
  palette.setColor( QPalette::AlternateBase, item_color );
  setPalette( palette );
  setAlternatingRowColors( true );
  
}
