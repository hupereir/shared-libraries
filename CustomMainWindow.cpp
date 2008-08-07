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
  \file CustomMainWindow.cpp
  \brief customized QMainWindow
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include "CustomMainWindow.h"
#include "CustomToolButton.h"

using namespace std;

//____________________________________________________________
CustomMainWindow::CustomMainWindow( QWidget *parent, Qt::WFlags wflags):
  QMainWindow( parent, wflags )
{ 
  Debug::Throw( "CustomMainWindow::CustomMainWindow.\n" );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
}


//__________________________________________________
QSize CustomMainWindow::minimumSizeHint( void ) const
{
  
  // resize
  if( _hasSizeOptionName() && XmlOptions::get().find( _widthOptionName() ) && XmlOptions::get().find( _heightOptionName() ) )
  {
    Debug::Throw() << " CustomMainWindow::minimumSizeHint - " << _widthOptionName() << ": " << XmlOptions::get().get<int>( _widthOptionName() ) << endl;
    Debug::Throw() << " CustomMainWindow::minimumSizeHint - " << _heightOptionName() << ": " << XmlOptions::get().get<int>( _heightOptionName() ) << endl;
    return QSize( 
      XmlOptions::get().get<int>( _widthOptionName() ), 
      XmlOptions::get().get<int>( _heightOptionName() ) );
  } else return QMainWindow::minimumSizeHint();
 
}

//__________________________________________________
QSize CustomMainWindow::sizeHint( void ) const
{
    
  // resize
  if( _hasSizeOptionName() && XmlOptions::get().find( _widthOptionName() ) && XmlOptions::get().find( _heightOptionName() ) )
  {
    Debug::Throw() << " CustomMainWindow::sizeHint - " << _widthOptionName() << ": " << XmlOptions::get().get<int>( _widthOptionName() ) << endl;
    Debug::Throw() << " CustomMainWindow::sizeHint - " << _heightOptionName() << ": " << XmlOptions::get().get<int>( _heightOptionName() ) << endl;
    return QSize( 
      XmlOptions::get().get<int>( _widthOptionName() ), 
      XmlOptions::get().get<int>( _heightOptionName() ) );
  } else return QMainWindow::sizeHint();
 
}

//____________________________________________________________
void CustomMainWindow::resizeEvent( QResizeEvent* event )
{
  if( _hasSizeOptionName() ) resize_timer_.start( 200, this );
  return QMainWindow::resizeEvent( event );
}

//_______________________________________________________
void CustomMainWindow::timerEvent( QTimerEvent* event )
{

  if( event->timerId() == resize_timer_.timerId() )
  {
    
    // stop timer
    assert( _hasSizeOptionName() );
    resize_timer_.stop();
    
    // save size
    XmlOptions::get().set<int>( _widthOptionName(), width() );
    XmlOptions::get().set<int>( _heightOptionName(), height() );
  
  } else return QMainWindow::timerEvent( event );
  
}

//____________________________________________________________
void CustomMainWindow::_updateConfiguration( void )
{ 
  
  Debug::Throw( "CustomMainWindow::_updateConfiguration.\n" );
  
  // pixmap size
  if( XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ) setIconSize( CustomToolButton::BigIconSize );
  else setIconSize( CustomToolButton::SmallIconSize );
  
  // text label for toolbars
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );
  
}
