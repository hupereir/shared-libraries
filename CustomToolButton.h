#ifndef CustomToolButton_h
#define CustomToolButton_h

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
  \file CustomToolButton.h
  \brief customized tool button to display tooltip in a dedicated label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QIconSet>
#include <QLabel>
#include <QToolButton>

#include <string>

#include "Counter.h"  
#include "Debug.h"  

/*!
  \class CustomToolButton
  \brief customized tool button to display tooltip in a dedicated label
*/
class CustomToolButton: public QToolButton, public Counter
{ 
  
  Q_OBJECT
  
  public:
  
  //! large icon size
  static const QSize BigIconSize;

  //! small icon size
  static const QSize SmallIconSize;
    
  //! default creator
  CustomToolButton( QWidget* parent );
     
  //! destructor
  virtual ~CustomToolButton( void ) 
  { Debug::Throw( "CustomToolButton::~CustomToolButton.\n" ); }
     
  //! small icon size
  const QSize& smallIconSize( void ) const
  { return small_icon_size_; }

  //! big icon size
  const QSize& bigIconSize( void ) const
  { return big_icon_size_; }
  
  //! icon size enumeration
  /*! used notably to assign values to a given icon size */
  enum IconSize
  {
    //! select big icons
    BIG = 1<<0,

    //! select small icons
    SMALL = 1<<1,
    
    //! select both sets of icons
    ALL = BIG|SMALL
    
  };
    
  //! small icon size
  void setSmallIconSize( const QSize& size )
  { setIconSize( size, SMALL ); }
  
  //! big icon size
  void setBigIconSize( const QSize& size )
  { setIconSize( size, BIG ); }
   
  //! icon size
  void setIconSize( const QSize& size, const IconSize& type )
  { 
    if( type & BIG ) big_icon_size_ = size; 
    if( type & SMALL ) small_icon_size_ = size; 
    _updateConfiguration();
  }
  
  //! rotation
  enum Rotation
  {
    NONE,
    CLOCKWISE,
    COUNTERCLOCKWISE
  };
  
  //! rotation
  void setRotation( const Rotation& value )
  { rotation_ = value; }
  
  //! size hint
  virtual QSize sizeHint( void ) const;
  
  protected:
  
  //! painting
  virtual void paintEvent( QPaintEvent* );
  
  private slots:
  
  //! tool button configuration
  void _updateConfiguration( void );
  
  private:
  
  //! rotation
  const Rotation& _rotation( void ) const
  { return rotation_; }
  
  //! big icon-size
  /*! by default this is the global BigIconSize but can be manually tweaked */
  QSize big_icon_size_;
  
  //! small icon-size
  /*! by default this is the global SmallIconSize but can be manually tweaked */
  QSize small_icon_size_;
  
  //! rotation
  Rotation rotation_;
  
};

#endif
