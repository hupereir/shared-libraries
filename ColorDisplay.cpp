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

#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>

#include <QHBoxLayout>
#include <QPainter>
#include <QToolButton>

#include "BaseIcons.h"
#include "ColorDisplay.h"
#include "Debug.h"
#include "IconEngine.h"
#include "InformationDialog.h"

using namespace std;

//______________________________________________
const QString ColorDisplay::NONE = "None";

//______________________________________________
ColorDisplay::ColorDisplay( QWidget* parent ):
  QWidget( parent ),
  Counter( "ColorDisplay" ),
  editor_( this ),
  locked_( false ),
  mouse_down_( false )
{
  Debug::Throw( "ColorDisplay::ColorDisplay.\n" );

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(2  );
  setLayout( layout );
  
  editor_.setHasClearButton( false );
  editor_.setAlignment( Qt::AlignCenter );
  layout->addWidget( &editor_, 1 );
  connect( &editor_, SIGNAL( returnPressed() ), SLOT( _selectColorFromText() ) );  

  // browse button
  QToolButton *button;
  layout->addWidget( button = new QToolButton( this ), 0 );
  button->setIcon( IconEngine::get( ICONS::OPEN ) );
  button->setToolTip( "select color from dialog" );
  //button->setAutoRaise( true );
  connect( button, SIGNAL( clicked() ), SLOT( _selectColor() ) );
  
  // grab button
  layout->addWidget( button = new QToolButton( this ), 0 );
  button->setIcon( IconEngine::get( ICONS::COLOR_PICKER ) );
  button->setToolTip( "grap color from screen" );
  //button->setAutoRaise( true );
  connect( button, SIGNAL( clicked() ), SLOT( _grabColor() ) );
  
} 

//________________________________________________________
void ColorDisplay::setColor( const QString& color )
{ 
  Debug::Throw( "ColorDisplay::setColor.\n" );
  editor_.setText( color );
  editor_.setColor( color == NONE ? palette().color( QPalette::Window ):QColor(color) );
}

//_____________________________________________________________
void ColorDisplay::mousePressEvent( QMouseEvent *event )
{
  Debug::Throw( "ColorDisplay::mousePressEvent.\n" );
  
  // check button
  if( event->button() != Qt::LeftButton ) return QWidget::mousePressEvent( event );
    
  // do nothing if mouse is not locked
  if( !locked_ ) return QWidget::mousePressEvent( event );

  // do nothing if mouse is already down
  if( mouse_down_ ) return;
  mouse_down_ = true;

}
  
//_____________________________________________________________
void ColorDisplay::mouseReleaseEvent( QMouseEvent *event )
{
  Debug::Throw( "ColorDisplay::mouseReleaseEvent.\n" );
    
  // do nothing if mouse is not locked
  if( !locked_ ) return QWidget::mouseReleaseEvent( event );
  
  // check button
  if( event->button() == Qt::LeftButton && mouse_down_ )
  {  
    // get color under mouse
    _selectColor( event );
  }
  
  mouse_down_ = false;
  locked_ = false;
  releaseMouse();
  
  // restore original cursor
  qApp->restoreOverrideCursor( ); 
  
}

//_____________________________________________________________
void ColorDisplay::mouseMoveEvent( QMouseEvent *event )
{
  
  Debug::Throw( "ColorDisplay::mouseMoveEvent.\n" );
  
  // do nothing if mouse is not locked
  if( !( locked_ && mouse_down_ ) ) return QWidget::mouseMoveEvent( event );
  _selectColor( event );

}

//________________________________________________________
void ColorDisplay::_selectColor( void )
{ 
  Debug::Throw( "ColorDisplay::_selectColor.\n" );
  QColor color( QColorDialog::getColor( editor_.color(), this ) );
  if( color.isValid() ) setColor( color.name() ); 
}


//_____________________________________________________________
void ColorDisplay::_selectColor( QMouseEvent *event )
{
  Debug::Throw() << "ColorDisplay::_SelectColor - (" << event->globalX() << "," << event->globalY() << ")" << endl;
    
  // grab desktop window under cursor
  // convert to image.
  QImage image( QPixmap::grabWindow(QApplication::desktop()->winId(),event->globalX(), event->globalY(), 2, 2 ).toImage() );

  // ensure image is deep enough
  if (image.depth() != 32) image = image.convertToFormat(QImage::Format_RGB32);
  
  // assign color to the selection frame
  setColor( QColor( image.pixel( 1, 1 ) ).name() );
  
  return;
}

//________________________________________________________
void ColorDisplay::_grabColor( void )
{ 
  
  Debug::Throw( "ColorDisplay::_grabColor.\n" );
  qApp->setOverrideCursor( Qt::CrossCursor );
  grabMouse();
  locked_ = true;

}

//________________________________________________________
void ColorDisplay::_selectColorFromText( void )
{ 
  Debug::Throw( "ColorDisplay::_selectColorFromText.\n" );
  
  QColor color;
  QString text = editor_.text();
  
  if( text != NONE )
  {
    color = QColor( text );
    if( !color.isValid() ) 
    {
      
      ostringstream what;
      if( !( text.isNull() || text.isEmpty() ) ) what << "Invalid color: " << qPrintable( text );
      else what << "Invalid color";
      
      InformationDialog( this, what.str().c_str() ).exec();
      editor_.setText( NONE );
    }
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
  QString text( LineEditor::text() );
  if( text == ColorDisplay::NONE || text.isEmpty() ) return QColor();
  else return QColor( text );
}

//_________________________________________________________
void ColorDisplay::LocalLineEdit::setColor( QColor color )
{
  Debug::Throw( "ColorDisplay::LocalLineEdit::setColor.\n" );
  
  // check color validity
  if( !color.isValid() ) 
  {
    
    ostringstream what;
    if( !( text().isNull() || text().isEmpty() ) ) what << "Invalid color: " << qPrintable( text() );
    else what << "Invalid color";
    
    InformationDialog( this, what.str().c_str() ).exec();
    
    // update text
    setText( NONE );
    
    // reassign color
    color = QLineEdit().palette().color( QPalette::Base );
    
  }

  // retrieve palette, change background/foreground
  QPalette palette( ColorDisplay::LocalLineEdit::palette() );
  palette.setColor( QPalette::Base, color );  
  palette.setColor( QPalette::Text, (color.value() >= 175) ? Qt::black:Qt::white );
  
  // update
  setPalette(palette);
  update();
  
}
