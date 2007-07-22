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
  \file CustomPixmap.h
  \brief customized QPixmap to look for icon of given name in list of directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef CustomPixmap_h
#define CustomPixmap_h

#include <list>
#include <string>
#include <QPixmap>

#include "Counter.h"


//! customized QPixmap to look for icon of given name in list of directory
class CustomPixmap: public QPixmap, public Counter
{
  
  public:
  
  //! constructor
  CustomPixmap( const QPixmap& pixmap ):
    QPixmap( pixmap ),
    Counter( "CustomPixmap" )
  {}
      
          
  //! constructor
  CustomPixmap( const std::string& file = "" ):
    QPixmap( file.c_str() ),
    Counter( "CustomPixmap" )
  {}
  
  //! destructor
  virtual ~CustomPixmap() 
  {}
  
  //! find first file matching name in list of path
  virtual CustomPixmap find( 
    const std::string& file, 
    const std::list< std::string >& path, 
    bool case_sensitive = true );
  
  //! resize
  virtual CustomPixmap scale( const unsigned int& width, const unsigned int& height ) const;
  
  //! resize
  virtual CustomPixmap scale( const QSize& size ) const
  { return scale( size.width(), size.height() ); }
  
  //! resize
  virtual CustomPixmap scaleWidth( const int& width ) const
  { return scale( width, (width*this->height())/this->width() ); }

  //! resize
  virtual CustomPixmap scaleHeight( const int& height ) const
  { return scale( (height*this->width())/this->height(), height ); }
  
  //! returns a tinted pixmap
  virtual CustomPixmap tint( const QColor& color, const double& intensity ) const;
  
  //! returns a tinted pixmap
  virtual CustomPixmap merge( const QPixmap& ) const;
  
  //! returns an empty pixmap of given size, of given color and possibly transparent
  static CustomPixmap empty( 
    const QSize& size, 
    const QColor& color = Qt::black, 
    const bool& transparent = true );
    
};

#endif
