#ifndef CustomToolBar_h
#define CustomToolBar_h

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
  \file CustomToolBar.h
  \brief customized tool bar to handle position and visibility from Options
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QMainWindow>
#include <QHideEvent>
#include <QShowEvent>
#include <QToolBar>
#include <list>
#include <map>

#include "Counter.h"
#include "Debug.h"
#include "OptionComboBox.h"

//! customized tool bar to handle position and visibility from Options
class CustomToolBar: public QToolBar, public Counter
{
  
  Q_OBJECT
    
  public:
    
  //! constructor
  CustomToolBar( const QString& title, QWidget* parent, const std::string& option_name = "" );
  
  //! destructor
  virtual ~CustomToolBar( void );
  
  //! get area from name
  static Qt::ToolBarArea nameToArea( const std::string& name )
  { 
    std::map< std::string, Qt::ToolBarArea>::iterator iter = toolbar_areas_.find( name );
    return iter == toolbar_areas_.end() ? (Qt::ToolBarArea) 0 :iter->second;
  }
  
  //! get name from toobar area
  static std::string areaToName( const Qt::ToolBarArea& value )
  { 
    
    for( std::map< std::string, Qt::ToolBarArea>::iterator iter = toolbar_areas_.begin(); iter != toolbar_areas_.end(); iter++ )
    { if( iter->second == value ) return iter->first; }
   
    return "";
    
  }
  
  //! lock from options
  const bool& lockFromOptions( void ) const
  { return lock_from_options_; }
  
  //! set movable
  /* when called directly from the application, the lock_from_options_ flag is set to false */
  void setMovable( bool value )
  {
    lock_from_options_ = false;
    return QToolBar::setMovable( value );
  }
  
  //! visibility action
  QAction& visibilityAction( void ) const
  { return *visibility_action_; }
  
  //! location option combo box
  class LocationComboBox: public OptionComboBox
  {
    
    public:
    
    //! constructor
    LocationComboBox( QWidget* parent, const std::string& option ):
        OptionComboBox( parent, option )
    {
      addItem( CustomToolBar::areaToName( Qt::TopToolBarArea ).c_str() );
      addItem( CustomToolBar::areaToName( Qt::BottomToolBarArea ).c_str() );
      addItem( CustomToolBar::areaToName( Qt::LeftToolBarArea ).c_str() );
      addItem( CustomToolBar::areaToName( Qt::RightToolBarArea ).c_str() );
    }
    
  };
  
  protected:
  
  //! show event
  virtual void showEvent( QShowEvent* );
  
  //! hide event
  virtual void hideEvent( QHideEvent* );
    
  //! move event
  virtual void moveEvent( QMoveEvent* );
  
  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
  
  //! toggle visibility
  void _toggleVisibility( bool );
  
  private:
  
  //! install actions
  void _installActions( void );
  
  //! initialize area map
  static bool _initializeAreas( void );
  
  //! assocated option name
  std::string option_name_;
  
  //! visibility action
  QAction* visibility_action_;
  
  //! use lock from options
  bool lock_from_options_;
  
  //! map name and toolbar are
  static std::map< std::string, Qt::ToolBarArea> toolbar_areas_;

  //! keep track of map initialization
  static bool initialized_;
  
};

#endif
