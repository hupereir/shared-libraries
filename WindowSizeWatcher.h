#ifndef WindowSizeWatcher_h
#define WindowSizeWatcher_h

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
  \file WindowSizeWatcher.h
  \brief used to automatically save size of top level widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QBasicTimer>
#include <QWidget>
#include <QSize>
#include <QTimerEvent>

#include "Counter.h"

//! used to automatically save size of top level widgets
class WindowSizeWatcher: public QObject, public Counter
{
  
  public:
  
  //! constructor
  WindowSizeWatcher( QWidget* parent ):
    QObject( parent ),
    Counter( "WindowSizeWatcher" )
  {}
    
  //! size option name
  virtual void setSizeOptionName( const std::string& name )
  { 
    width_option_name_ = name + "_WIDTH"; 
    height_option_name_ = name + "_HEIGHT"; 
  }
    
  //! size hint
  /*! an invalid size is returned when option name is not set */
  QSize sizeHint( void ) const;
  
  //! restart timer 
  /*! it is needed to save window size during resize events */
  void restart( void );
  
  protected:
    
  //! timer event
  /* need to save updated window size */
  virtual void timerEvent( QTimerEvent* );     
  
  private:

  //! true when option name was set
  bool _hasSizeOptionName( void ) const
  { return !width_option_name_.empty(); }

  //! size option name
  virtual const std::string _heightOptionName( void ) const
  { return height_option_name_; }
  
  //! size option name
  virtual const std::string _widthOptionName( void ) const
  { return width_option_name_; }

  //! save window size
  void _saveWindowSize( void ) const;

  //! resize timer
  QBasicTimer resize_timer_;
  
  //! window size option name
  std::string width_option_name_;

  //! window size option name
  std::string height_option_name_;
  
};

#endif
