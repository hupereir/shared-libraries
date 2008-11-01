#ifndef QtUtil_h
#define QtUtil_h
 
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
  \file QtUtil.h
  \brief Qt utility static methods
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QDialog>
#include <string>


/*!
   \class QtUtil
   \brief Qt utility static methods
*/

class QtUtil {
  public:
  
  //! dimension to be fixed
  enum FixMode {
    
    //! widget can be extended in both directions
    NONE = 0,
        
    //! widget has fixed width
    WIDTH = 1,
        
    //! widget has fixed height
    HEIGHT = 2
  };
    
  //! move menu to position taking proper care of the screen borders
  static void moveWidget( QWidget* widget, QPoint position );
 
  //! center widget on pointer
  static QPoint centerOnPointer( const QSize& size );
     
  //! return topleft point so that size is centered on widget
  static QPoint centerOnWidget( const QSize& size, QWidget* parent );
    
  //! return topleft point so that size is centered on pointer
  static QPoint centerOnDesktop( const QSize& size );

  //! center widget on pointer
  static void centerOnPointer( QWidget* widget );
 
  //! center widget on parent top level widget
  static void centerOnParent( QWidget* widget );
  
  //! center widget on parent top level widget
  static void centerOnWidget( QWidget* widget, QWidget* parent );
  
  //! center widget on desktop widget
  static void centerOnDesktop( QWidget* widget );

  //! uniconify a top level widget
  static void uniconify( QWidget* widget );
  
  protected:
      
  //! maximum value for opacity setting (corresponds to solid)
  static const unsigned int max_opacity_;
  
  //! opacity property name
  static const char* opacity_prop_name_;

};

#endif
