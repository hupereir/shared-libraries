#ifndef CustomMainWindow_h
#define CustomMainWindow_h

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
  \file CustomMainWindow.h
  \brief customized QMainWindow
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include <QAction>
#include <QBasicTimer>
#include <QMainWindow>
#include <QResizeEvent>
#include <QTimerEvent>

#include "Debug.h"  
#include "XmlOptions.h"  

/*!
  \class CustomMainWindow
  \brief customized QDialog
*/
class CustomMainWindow: public QMainWindow
{
  
  Q_OBJECT

  public:
  
  //! constructor
  CustomMainWindow( QWidget *parent, Qt::WFlags wflags = 0);
  
  //! destructor
  virtual ~CustomMainWindow( void )
  { Debug::Throw( "CustomMainWindow::~CustomMainWindow.\n" ); }

  //! restore window size
  virtual QSize minimumSizeHint( void ) const;

  //! restore window size
  virtual QSize sizeHint( void ) const;
  
  //! lock toolbars
  QAction& lockToolBarsAction( void ) const
  { 
    assert( lock_toolbars_action_ );
    return *lock_toolbars_action_; 
  }
  
  protected:
  
  //! resize event
  /* need to save updated window size */
  virtual void resizeEvent( QResizeEvent* );
  
  //! timer event
  /* need to save updated window size */
  virtual void timerEvent( QTimerEvent* );     
    
  //! size option name
  void _setSizeOptionName( const std::string& name )
  { 
    width_option_name_ = name + "_WIDTH"; 
    height_option_name_ = name + "_HEIGHT"; 
  }
  
  //! true when option name was set
  bool _hasSizeOptionName( void ) const
  { return !width_option_name_.empty(); }
  
  //! size option name
  const std::string _heightOptionName( void ) const
  { return height_option_name_; }
  
  //! size option name
  const std::string _widthOptionName( void ) const
  { return width_option_name_; }

  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
  
  //! save configuration
  void _saveConfiguration( void );

  //! lock toolbars
  void _lockToolBars( bool ); 
  
  private:
    
  //! lock toolbars
  QAction* lock_toolbars_action_;
  
  //! resize timer
  QBasicTimer resize_timer_;
  
  //! window size option name
  std::string width_option_name_;

  //! window size option name
  std::string height_option_name_;
  
};

#endif 
    
