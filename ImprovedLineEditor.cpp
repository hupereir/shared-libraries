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
  \file ImprovedLineEditor.cpp
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrameV2>

#include "BaseIcons.h"
#include "Debug.h"
#include "IconEngine.h"
#include "ImprovedLineEditor.h"
#include "QtUtil.h"

using namespace std;

//______________________________________________________________
ImprovedLineEditor::ImprovedLineEditor( QWidget* parent ):
  QFrame( parent ),
  Counter( "ImprovedLineEditor" )
{
  Debug::Throw( "ImprovedLineEditor::ImprovedLineEditor.\n" );
  
  setFrameStyle( QFrame::StyledPanel|QFrame::Sunken );
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  setLayout( layout );
  
  // editor
  layout->addWidget( editor_ = new LineEditor( this ), 1 );
  editor_->setFrame( false );
  
  // clear button
  layout->addWidget( clear_button_ = new Button( this ), 0 );
  clear_button_->setIcon( IconEngine::get( ICONS::CLEAR ) );
  int width = editor().fontMetrics().lineSpacing();
  clear_button_->setFixedSize( width + 1, width );
  
  // connections
  connect( &editor(), SIGNAL( returnPressed() ), SIGNAL( returnPressed() ) );
  connect( &editor(), SIGNAL( cursorPositionChanged( int ) ), SIGNAL( cursorPositionChanged( int ) ) );
  connect( &editor(), SIGNAL( modificationChanged( bool ) ), SIGNAL( modificationChanged( bool ) ) );
  connect( &editor(), SIGNAL( cursorPositionChanged( int, int ) ), SIGNAL( cursorPositionChanged( int, int ) ) );
  connect( &editor(), SIGNAL( textChanged( const QString& ) ), SLOT( _textChanged( const QString& ) ) );
  connect( &editor(), SIGNAL( textChanged( const QString& ) ), SIGNAL( textChanged( const QString& ) ) );
  connect( qApp, SIGNAL( focusChanged( QWidget*, QWidget* ) ), SLOT( update() ) );

  connect( clear_button_, SIGNAL( clicked() ), &editor(), SLOT( clear() ) );
  
  // size
  QtUtil::fixSize( this, QtUtil::HEIGHT );
  
  // hide button by default since text is empty
  _clearButton().hide();
}

//________________________________________________
void ImprovedLineEditor::paintEvent( QPaintEvent* event )
{
  
  Debug::Throw( "ImprovedLineEditor::paintEvent.\n" );

  // this is a hack from QFrame::paintEvent
  // to handle focus.  
  QPainter painter(this);
  painter.save();
  painter.setPen( Qt::NoPen );
  painter.setBrush( editor().palette().color( QPalette::Base ) );
  QRect rect( frameRect() );
  int offset( frameWidth() );
  rect.adjust( offset, offset, -offset, -offset );
  painter.drawRect( rect );
  painter.restore();
  
  QStyleOptionFrameV2 panel;
  panel.initFrom( this );
  panel.rect = frameRect();
  panel.state |= QStyle::State_Sunken;
  if( editor().hasFocus() ) panel.state |= QStyle::State_HasFocus;
  style()->drawPrimitive(QStyle::PE_Frame, &panel, &painter, this);
  painter.end();
  
}

//________________________________________________
void ImprovedLineEditor::_textChanged( const QString& text )
{
  if( text.isEmpty() )
  {
    if( _clearButton().isVisible() ) _clearButton().hide();
    return;
  } else if( !_clearButton().isVisible() ) _clearButton().show();
  return;
}

//________________________________________________
void ImprovedLineEditor::Button::paintEvent( QPaintEvent* event )
{
  
  QPainter painter( this );
  QRect rect( this->rect() );
  rect.adjust( 0, -1, 0, 1 );
  icon().paint( &painter, rect );
  painter.end();
  
}
