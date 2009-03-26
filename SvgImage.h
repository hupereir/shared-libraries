#ifndef SvgImage_h
#define SvgImage_h

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
  \file SvgImage.h
  \brief construct pixmap of given size using Svg renderer
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QImage>
#include <QSvgRenderer>
#include <map>

#include "Counter.h"

//! construct pixmap of given size using Svg renderer
namespace SVG
{
  class SvgImage: public QImage
  {
    
    public:
     
    //! constructor
    SvgImage( const QSize& size = QSize() );
    
    //! render
    SvgImage& render( QSvgRenderer&, const double& offset = 0 );
    
    //! destructor
    virtual ~SvgImage( void )
    {}

    //! ordered QSize subclass
    class Size: public QSize
    {
      
      public:
      
      //! constructor
      Size( const QSize& size ):
        QSize( size )
      {}
      
      // order
      bool operator < ( const Size& size )  const
      { return ( width() < size.width() || ( width() == size.width() && height() < size.height() ) ); }
      
    };
  
    //! map size and pixmap
    typedef std::map< Size, SvgImage > Cache;

  };

};
#endif
