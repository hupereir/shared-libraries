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
  \file CustomListBox.cpp
  \brief qlistbox with even objects highlighted with different color
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QPainter>

#include "ColorDisplay.h"
#include "CustomListBox.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
CustomListBox::CustomListBox( QWidget* parent ):
  QListWidget( parent ),
  Counter( "CustomListBox" )
{
  
  Debug::Throw( "CustomListBox::CustomListBox.\n" ); 
  updateItemColor();
  
  setItemDelegate ( new Delegate( this ) );
  
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( updateItemColor() ) );
  
}  

//_______________________________________________
QListWidgetItem& CustomListBox::findItem( const std::string& name )
{
  QList<QListWidgetItem*> items( findItems( name.c_str(), Qt::MatchExactly ) );
  return ( !items.empty() ) ? *items.front() : *(new QListWidgetItem( name.c_str(), this, 0 ) );  
}


//_____________________________________________________________________
void CustomListBox::updateItemColor( void )
{
  
  QColor item_color;
  
  // try load from option
  Str colorname( XmlOptions::get().get<string>("ITEM_COLOR").c_str() );
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

//_____________________________________________________________________
void CustomListBox::Delegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
  
  Debug::Throw( "CustomListBox::Delegate::Paint.\n" );   
  QStyleOptionViewItem new_option( option );
  
  //  QGradient 
  QLinearGradient gradient(QPointF(0, 0), QPointF(painter->device()->width(), 0));
  QColor color( option.palette.color( QPalette::Highlight ) );
  gradient.setColorAt(0, color.light(130) );
  gradient.setColorAt(0.3, color );
  gradient.setColorAt(1, color.light(130) );
  new_option.palette.setBrush( QPalette::Highlight, QBrush( gradient ) );
  
  return QItemDelegate::paint( painter, new_option, index );
}
