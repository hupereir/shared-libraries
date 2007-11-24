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
\file ColorDisplay.cpp
\brief used to display colors and a choose button
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>

#include "ColorDisplay.h"
#include "Debug.h"
#include "QtUtil.h"


using namespace std;

//______________________________________________
const QString ColorDisplay::NONE = "None";

//______________________________________________
ColorDisplay::ColorDisplay( QWidget* parent ):
  QWidget( parent ),
  Counter( "ColorDisplay" ),
  editor_( this )
{
  Debug::Throw( "ColorDisplay::ColorDisplay.\n" );

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(5);
  setLayout( layout );
  
  editor_.setAlignment( Qt::AlignCenter );
  QtUtil::expand( &editor_, " #ffffff " ); 
  layout->addWidget( &editor_ );
  connect( &editor_, SIGNAL( returnPressed() ), SLOT( _changeColorFromText() ) );  
  
  QPushButton *button( new QPushButton( "...", this ) );
  QtUtil::fixSize( button );
  layout->addWidget( button );
  
  connect( button, SIGNAL( clicked() ), SLOT( _changeColor() ) );
  
} 

//________________________________________________________
void ColorDisplay::setColor( const QString& color )
{ 
  Debug::Throw( "ColorDisplay::setColor.\n" );
  editor_.setColor( color == NONE ? palette().color( QPalette::Window ):QColor(color) );
  editor_.setText( color );
}

//________________________________________________________
void ColorDisplay::_changeColor( void )
{ 
  Debug::Throw( "ColorDisplay::_changeColor.\n" );
  QColor color( QColorDialog::getColor( editor_.color(), this ) );
  if( color.isValid() ) setColor( color.name() ); 
}

//________________________________________________________
void ColorDisplay::_changeColorFromText( void )
{ 
  Debug::Throw( "ColorDisplay::_changeColorFromText.\n" );
  
  QColor color;
  QString text = editor_.text();
  
  if( text != NONE )
  {
    color = QColor( text );
    if( !color.isValid() ) QtUtil::infoDialog( this, "Invalid color" );
  }
  
  if( color.isValid() ) 
  {
    
    editor_.setColor( color );
    editor_.setText( color.name() );
  
  } else editor_.setColor( palette().color( QPalette::Window ) );
  
}

//_______________________________________________
QColor ColorDisplay::LocalLineEdit::color( void ) const
{ 
  Debug::Throw( "ColorDisplay::LocalLineEdit::color.\n" );
  QString text( CustomLineEdit::text() );
  if( text == ColorDisplay::NONE ) return QColor();
  else return QColor( text ); 
}

//_________________________________________________________
void ColorDisplay::LocalLineEdit::setColor( QColor color )
{
  Debug::Throw( "ColorDisplay::LocalLineEdit::setColor.\n" );
  
  if( !color.isValid() ) {
    setText( color.name() );
    QtUtil::infoDialog( this, "invalid color" );
    return;
  }

  // retrieve palette, change background/foreground
  QPalette local_palette( palette() );
  local_palette.setColor( QPalette::Base, color );  
  local_palette.setColor( QPalette::Text, (color.value() >= 175) ? Qt::black:Qt::white );
  
  // update
  setPalette(local_palette);
  update();
  
}

