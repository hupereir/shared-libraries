#ifndef _BackgroundPixmap_h_
#define _BackgroundPixmap_h_

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
   \file    BackgroundPixmap.h
   \brief   stores/manipulates the X11 background Pixmap
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QObject>
#include <QPixmap>

#include "Counter.h"

//! stores/manipulates the X11 background Pixmap
namespace TRANSPARENCY
{
  
  //! stores/manipulates the X11 background Pixmap
  class BackgroundPixmap: public QObject, public Counter
  {
    
    //! Qt meta object declaration
    Q_OBJECT
      
    public:
      
    //! returns singleton
    static BackgroundPixmap& get( void )
    { 
      if( !singleton_ ) singleton_ = new BackgroundPixmap();
      return *singleton_;
    }
    
    //! returns true if background image is valid
    bool isValid( void ) const
    { return !background_pixmap_.isNull(); }
    
    //! returns a Pixmap of given position and size from the background image
    QPixmap pixmap( const QRect& rect ) const
    { return background_pixmap_.copy( rect ); }

    //! returns a Pixmap of given position and size from the background image
    QPixmap fullPixmap() const
    { return background_pixmap_; }
    
    //! check X11 event
    #ifdef Q_WS_X11
    bool x11Event( XEvent* event );
    #endif
   
    public slots:
    
    //! loads the background image internaly
    void reload( void );
    
    signals:
    
    //! emited when background is changed  
    void backgroundChanged( void );
    
    private: 
    
    //! creator
    BackgroundPixmap( void );
    
    //! try load the desktopwindow tp which arg widget belongs
    bool _loadDesktopWindow( void );
       
    //! singleton
    static BackgroundPixmap *singleton_;
    
    //! full background image
    QPixmap background_pixmap_;

    #ifdef Q_WS_X11
 
    //! desktop window id
    WId desktop_;
    
    /* 
    the additional _BackgroundPixmap_p_ define is used to hide part of the class
    to the rest of the world, except itself. This allows to avoid having #includes 
    above that conflict with the Qt includes. They must be kept at the end of the Class
    to avoid wrong object size at linking.
    */
    #ifdef _BackgroundPixmap_p_
    
    //! get lowest bit (shift) from integer value (little/big endianness)
    int _lowestBit( const unsigned int& val );

    // pointer to pixel converter
    typedef QRgb (*PixelConverter) (XImage* image, const unsigned int& x, const unsigned int& y);
  
    //! get image format
    PixelConverter _converter( XImage* image );
    
    //! X11 format rotation
    static QRgb _convertPixel_BIT32( XImage* image, const unsigned int& x, const unsigned int& y );
    
    //! X11 format rotation
    static QRgb _convertPixel_BIT16_RGB_565( XImage* image, const unsigned int& x, const unsigned int& y );
    
    //! X11 format rotation
    static QRgb _convertPixel_BIT16_RGB_555( XImage* image, const unsigned int& x, const unsigned int& y );
    
    //! X11 format rotation
    static QRgb _convertPixel_BIT8( XImage* image, const unsigned int& x, const unsigned int& y );
    
    //! X11 format rotation
    static QRgb _convertPixel_UNKNOWN( XImage* image, const unsigned int& x, const unsigned int& y );

    //! root pixmap atom
    Atom root_pixmap_atom_;
    
    #endif
    #endif

    
  };
  
};
#endif
  
