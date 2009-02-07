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

#include <QHideEvent>
#include <QShowEvent>
#include <QToolBar>
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
  CustomToolBar( const QString& title = QString(), QWidget* parent = 0, const QString& option_name = "" );
  
  //! destructor
  virtual ~CustomToolBar( void );

  // map toolbar area and name
  typedef std::map< QString, Qt::ToolBarArea> AreaMap; 
  
  //! get area from name
  static Qt::ToolBarArea nameToArea( const QString& name )
  { 
    AreaMap::iterator iter = _toolbarAreas().find( name );
    return iter == _toolbarAreas().end() ? (Qt::ToolBarArea) 0 :iter->second;
  }
  
  //! get name from toobar area
  static QString areaToName( const Qt::ToolBarArea& value )
  { 
    
    for( AreaMap::iterator iter = _toolbarAreas().begin(); iter != _toolbarAreas().end(); iter++ )
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
  
  //! lock from options
  const bool& sizeFromOptions( void ) const
  { return size_from_options_; }
    
  //! set movable
  /* when called directly from the application, the lock_from_options_ flag is set to false */
  void setIconSize( const QSize& size )
  {
    size_from_options_ = false;
    return QToolBar::setIconSize( size );
  }
  
  //! visibility action
  QAction& visibilityAction( void ) const
  { return *visibility_action_; }
  
  //! location option combo box
  class LocationComboBox: public OptionComboBox
  {
    
    public:
    
    //! constructor
    LocationComboBox( QWidget* parent, const QString& option ):
        OptionComboBox( parent, option )
    {
      addItem( CustomToolBar::areaToName( Qt::TopToolBarArea ) );
      addItem( CustomToolBar::areaToName( Qt::BottomToolBarArea ) );
      addItem( CustomToolBar::areaToName( Qt::LeftToolBarArea ) );
      addItem( CustomToolBar::areaToName( Qt::RightToolBarArea ) );
    }
    
  };
  
  protected:
  
  //! show event
  virtual void showEvent( QShowEvent* );
  
  //! hide event
  virtual void hideEvent( QHideEvent* );
    
  //! move event
  virtual void moveEvent( QMoveEvent* );
  
  protected slots:

  //! toggle visibility
  virtual void _toggleVisibility( bool );
  
  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
    
  private:
  
  //! install actions
  void _installActions( void );
  
  //! assocated option name
  QString option_name_;
  
  //! visibility action
  QAction* visibility_action_;
  
  //! use icon size from options
  bool size_from_options_;
  
  //! use lock from options
  bool lock_from_options_;
  
  //! initialize area map
  static AreaMap _initializeAreas( void );

  //! map name and toolbar are
  static AreaMap& _toolbarAreas( void );
  
};

#endif
