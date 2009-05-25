#ifndef WindowMonitor_h
#define WindowMonitor_h

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
  \file WindowMonitor.h
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
class WindowMonitor: public QObject, public Counter
{
  
  public:
  
  //! constructor
  WindowMonitor( QWidget* );
  
  //! mode
  enum Mode
  {
    NONE = 0,
    SIZE = 1<<0,
    POSITION = 1<<1
  };
  
  //! mode
  void setMode( const unsigned int value )
  { mode_ = value; }
  
  //! size option name
  virtual void setOptionName( const QString& );
    
  //! size hint
  /*! an invalid size is returned when option name is not set */
  QSize sizeHint( void ) const;

  //! position
  QPoint position( void ) const;
    
  //! event filter
  virtual bool eventFilter( QObject*, QEvent* );
  
  protected:
    
  //! timer event
  /* need to save updated window size */
  virtual void timerEvent( QTimerEvent* );     
  
  private:

  //! mode
  const unsigned int& _mode( void ) const
  { return mode_; }
  
  //! true when option name was set
  bool _hasOptionName( void ) const
  { return !width_option_name_.isEmpty(); }

  //! size option name
  virtual const QString& _heightOptionName( void ) const
  { return height_option_name_; }
  
  //! size option name
  virtual const QString& _widthOptionName( void ) const
  { return width_option_name_; }

  //! x option name
  virtual const QString& _xOptionName( void ) const
  { return x_option_name_; }
  
  //! y option name
  virtual const QString& _yOptionName( void ) const
  { return y_option_name_; }
  
  //! save window size
  void _saveWindowSize( void ) const;

  //! save position
  void _saveWindowPosition( void ) const;
  
  //! resize timer
  QBasicTimer timer_;
  
  //! mode
  unsigned int mode_;
  
  //! window size option name
  QString width_option_name_;

  //! window size option name
  QString height_option_name_;
  
  //! position option name
  QString x_option_name_;
  
  //! position option name
  QString y_option_name_;

};

#endif
