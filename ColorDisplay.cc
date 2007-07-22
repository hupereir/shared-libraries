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
\file ColorDisplay.cc
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
ColorDisplay::ColorDisplay( QWidget* parent ):
  QWidget( parent ),
  Counter( "ColorDisplay" ),
  line_edit_( this )
{
  Debug::Throw( "ColorDisplay::ColorDisplay.\n" );

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(2);
  setLayout( layout );
  
  line_edit_.setAlignment( Qt::AlignCenter );
  QtUtil::expand( &line_edit_, " #ffffff " ); 
  layout->addWidget( &line_edit_ );
  connect( &line_edit_, SIGNAL( returnPressed() ), this, SLOT( _changeColorFromText() ) );  
  
  QPushButton *button( new QPushButton( "...", this ) );
  QtUtil::fixSize( button );
  layout->addWidget( button );
  
  connect( button, SIGNAL( clicked() ), this, SLOT( _changeColor() ) );
  
} 

//_________________________________________________________
void ColorDisplay::LocalLineEdit::setColor( const QColor& color )
{
  Debug::Throw( "ColorDisplay::LocalLineEdit::SetColor.\n" );
  
  if( !color.isValid() ) {
    QtUtil::infoDialogExclusive( this, "invalid color" );
    return;
  }

  // retrieve palette, change background/foreground
  QPalette local_palette( palette() );
  local_palette.setColor( QPalette::Base, color );  
  local_palette.setColor( QPalette::Text, (color.value() >= 175) ? Qt::black:Qt::white );
  
  // update
  setPalette(local_palette);
  setText( color.name() );
  
}

