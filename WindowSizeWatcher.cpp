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
  \file WindowSizeWatcher.cpp
  \brief used to automatically save size of top level widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include "XmlOptions.h"
#include "WindowSizeWatcher.h"

using namespace std;

//_________________________________________________________
void WindowSizeWatcher::setOptionName( const std::string& name )
{
  Debug::Throw( "WindowSizeWatcher::setOptionName.\n" );
  if( name.empty() )
  { 
    width_option_name_.clear();
    height_option_name_.clear();
  } else {
    width_option_name_ = name + "_WIDTH"; 
    height_option_name_ = name + "_HEIGHT"; 
  }
}

//_________________________________________________________
QSize WindowSizeWatcher::sizeHint( void ) const
{
  
  // resize
  if( _hasSizeOptionName() && XmlOptions::get().find( _widthOptionName() ) && XmlOptions::get().find( _heightOptionName() ) )
  {
    return QSize( 
      XmlOptions::get().get<int>( _widthOptionName() ), 
      XmlOptions::get().get<int>( _heightOptionName() ) );
  } else return QSize();

}

//_________________________________________________________
void WindowSizeWatcher::restart( void )
{
  if( _hasSizeOptionName() ) 
  { resize_timer_.start( 200, this ); }
}

//_______________________________________________________
void WindowSizeWatcher::timerEvent( QTimerEvent* event )
{

  if( event->timerId() == resize_timer_.timerId() )
  {
    
    // stop timer
    assert( _hasSizeOptionName() );
    resize_timer_.stop();
    
    // save size
    _saveWindowSize();
  
  } else return QObject::timerEvent( event );
  
}


//____________________________________________________________
void WindowSizeWatcher::_saveWindowSize( void ) const
{ 
  Debug::Throw( "WindowSizeWatcher::_saveWindowSize.\n" );
  XmlOptions::get().set<int>( _widthOptionName(), static_cast<QWidget*>(parent())->width() );
  XmlOptions::get().set<int>( _heightOptionName(), static_cast<QWidget*>(parent())->height() );
}
