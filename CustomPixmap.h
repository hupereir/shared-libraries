#ifndef CustomPixmap_h
#define CustomPixmap_h

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

#include <assert.h>
#include <list>

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
  CustomPixmap( const QImage& image ):
    Counter( "CustomPixmap" )
  { *this = fromImage( image ); }
                
  //! constructor
  CustomPixmap( void ):
    Counter( "CustomPixmap" )
    {}
    
  //! constructor
  CustomPixmap( const QString& file );
      
  //! destructor
  virtual ~CustomPixmap() 
  {}
  
  //! find first file matching name in list of path
  virtual CustomPixmap find( const QString& file );
     
  //! rotation
  enum Rotation
  {
    NONE,
    CLOCKWISE,
    COUNTERCLOCKWISE
  };
  
  //! rotation
  CustomPixmap rotate( const Rotation& value );
  
  //! returns a tinted pixmap
  virtual CustomPixmap tint( const QColor& color, const double& intensity ) const;
  
  //! corner enumeration for merging pixmap
  enum Corner
  {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CENTER
  };
  
  //! merge pixmap, using the specified corner as an anchor
  virtual CustomPixmap merge( const QPixmap&, Corner corner = TOP_LEFT ) const;
  
  //! returns an empty pixmap of given size, of given color and possibly transparent
  virtual CustomPixmap empty( 
    const QSize& size, 
    const QColor& color = Qt::transparent ) const;
  
  //! return greyed (inactive) pixmap, build from the current
  virtual CustomPixmap disabled( void );

  //! return highlighted (active) pixmap, build from the current
  virtual CustomPixmap active( void );
    
};

#endif
