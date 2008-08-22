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
#include <QActionGroup>
#include <QBasicTimer>
#include <QMainWindow>
#include <QMenu>
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
   
  //! create context menu (overloaded)
  virtual QMenu* createPopupMenu( void );
  
  //! install toolbar visibility actions
  /*! returns true if lockable toolbars are found */
  virtual bool installToolBarsActions( QMenu& );
  
  signals:
  
  //! toolbar configuration changed
  void toolbarConfigurationChanged( void );
  
  protected:
    
  //! resize event
  /* need to save updated window size */
  virtual void resizeEvent( QResizeEvent* );
  
  //! timer event
  /* need to save updated window size */
  virtual void timerEvent( QTimerEvent* );     
  
  //! save window size
  virtual void _saveWindowSize( void ) const;
  
  //! size option name
  virtual void _setSizeOptionName( const std::string& name )
  { 
    width_option_name_ = name + "_WIDTH"; 
    height_option_name_ = name + "_HEIGHT"; 
  }
  
  //! true when option name was set
  virtual bool _hasSizeOptionName( void ) const
  { return !width_option_name_.empty(); }
  
  //! size option name
  virtual const std::string _heightOptionName( void ) const
  { return height_option_name_; }
  
  //! size option name
  virtual const std::string _widthOptionName( void ) const
  { return width_option_name_; }

  //! true if main window has toolbars
  virtual bool _hasToolBars( void ) const;
  
  //! install toolbars text position actions
  virtual void _installToolButtonStyleActions( QMenu& );

  //! install icon size actions
  virtual void _installToolButtonIconSizeActions( QMenu& );
  
  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
 
  //! toolbar text position
  void _updateToolButtonStyle( QAction* );
 
  //! toolbar text position
  void _updateToolButtonIconSize( QAction* );
    
  //! lock toolbars
  void _lockToolBars( bool ); 
  
  private:
    
  //! lock toolbars
  QAction* lock_toolbars_action_;
  
  //! resize timer
  QBasicTimer resize_timer_;
  
  //! toolbar text actions group
  QActionGroup* toolbutton_style_group_;
  
  //! toolbar text actions group
  QActionGroup* icon_size_group_;
 
  //! action map
  typedef std::map<QAction*, int > ActionMap;
    
  //! toolbar text action map
  ActionMap toolbutton_style_actions_;
    
  //! toolbar text action map
  ActionMap icon_size_actions_;

  //! window size option name
  std::string width_option_name_;

  //! window size option name
  std::string height_option_name_;
  
};

#endif 
    
