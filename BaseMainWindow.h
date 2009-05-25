#ifndef BaseMainWindow_h
#define BaseMainWindow_h

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
  \file BaseMainWindow.h
  \brief customized QMainWindow
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include <QAction>
#include <QActionGroup>
#include <QMainWindow>
#include <QMenu>

#include "CustomToolButton.h"
#include "Debug.h"  
#include "IconSize.h"  
#include "WindowMonitor.h"  
#include "XmlOptions.h"  

class ToolBarMenu;

/*!
  \class BaseMainWindow
  \brief customized QDialog
*/
class BaseMainWindow: public QMainWindow
{
  
  Q_OBJECT

  public:
  
  //! constructor
  BaseMainWindow( QWidget *parent, Qt::WFlags wflags = 0);
  
  //! destructor
  virtual ~BaseMainWindow( void )
  { Debug::Throw( "BaseMainWindow::~BaseMainWindow.\n" ); }

  //! restore window size
  virtual QSize minimumSizeHint( void ) const;

  //! restore window size
  virtual QSize sizeHint( void ) const;

  //! center widget on argument widget
  void centerOnDesktop( void );
  
  //! center on parent widget
  void centerOnParent( void )
  { centerOnWidget( parentWidget() ); }
  
  //! center widget on argument widget
  void centerOnWidget( QWidget* );

  //! window title
  virtual void setWindowTitle( const QString& );

  //! set menu bar (overloaded)
  void setMenuBar( QMenuBar* );
  
  //! lock toolbars
  QAction& lockToolBarsAction( void ) const
  { 
    assert( lock_toolbars_action_ );
    return *lock_toolbars_action_; 
  }
  
  //! lock toolbars
  QAction& showMenuAction( void ) const
  { 
    assert( show_menu_action_ );
    return *show_menu_action_; 
  }

  //! create context menu (overloaded)
  virtual QMenu* createPopupMenu( void );

  //! toolbar menu
  virtual ToolBarMenu& toolBarMenu( QWidget* parent = 0 );
  
  //! install toolbar visibility actions
  /*! returns true if lockable toolbars are found */
  virtual bool installToolBarsActions( QMenu& );
   
  //! option name
  virtual void setOptionName( const QString& name );
      
  //! menu option name
  bool hasOptionName( void ) const
  { return !lockToolBarsOptionName().isEmpty(); }
      
  //! menu option name
  const QString& lockToolBarsOptionName( void ) const
  { return lock_toolbars_option_name_; }

  //! menu option name
  const QString& showMenuOptionName( void ) const
  { return show_menu_option_name_; }
  
  signals:
  
  //! toolbar configuration changed
  void toolbarConfigurationChanged( void );

  public slots:
  
  //! uniconify
  void uniconify( void );
  
  protected:
    
  //! generic event
  virtual bool event( QEvent* );

  //! true if main window has toolbars
  virtual bool _hasToolBars( void ) const;
 
  //! maximize state prior to minimization
  bool _wasMaximized( void ) const
  { return was_maximized_; }
  
  //! maximize state prior to minimization
  void _setWasMaximized( bool value )
  { was_maximized_ = value; }

  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
 
  //! toolbar text position
  void _updateToolButtonStyle( Qt::ToolButtonStyle );
 
  //! toolbar text position
  void _updateToolButtonIconSize( IconSize::Size );
    
  //! lock toolbars
  void _lockToolBars( bool ); 
  
  //! toggle menu
  void _showMenu( bool );
  
  private:
  
  //! option name
  QString lock_toolbars_option_name_;
  
  //! option name
  QString show_menu_option_name_;

  //! size watcher
  WindowMonitor monitor_;
  
  //! lock toolbars
  QAction* lock_toolbars_action_;

  //! toggle menu
  QAction* show_menu_action_;
  
  //! window state prior to minimization
  bool was_maximized_;
  
};

#endif 
    
