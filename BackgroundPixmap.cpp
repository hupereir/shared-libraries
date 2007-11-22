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
   \file    BackgroundPixmap.cpp
   \brief   stores/manipulates the X11 background Pixmap
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QApplication>
#include <QDesktopWidget> 

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

#define _BackgroundPixmap_p_
#include "BackgroundPixmap.h"
#include "Debug.h"

using namespace std;
  
//_____________________________________________________
using namespace TRANSPARENCY;
BackgroundPixmap* BackgroundPixmap::singleton_;
  


//_____________________________________________________
BackgroundPixmap::BackgroundPixmap( void ):
  Counter( "BackgroundPixmap" )
{ 
  Debug::Throw( "BackgroundPixmap::BackgroundPixmap.\n" );
  
  #ifdef Q_WS_X11
  desktop_ = 0;
  root_pixmap_atom_ = 0;
  #endif
  
  reload(); 
}
  

//_____________________________________________________
#ifdef Q_WS_X11
bool BackgroundPixmap::x11Event( XEvent* event )
{
  
  // cast event to a property event
  XPropertyEvent *property_event = (XPropertyEvent*) event;
  if( 
    property_event && 
    property_event->window == desktop_ &&
    property_event->atom == root_pixmap_atom_ )
  {
    Debug::Throw( "BackgroundPixmap::x11Event - property event.\n" );
    reload();
    return true;
  } else return false;
  
}
#endif

//_____________________________________________________
void BackgroundPixmap::reload( void )
{
  Debug::Throw( "BackgroundPixmap::reload.\n" );
    
  // reset background_pixmap_
  background_pixmap_ = QPixmap();
  
  #ifdef Q_WS_X11
  // try load desktop windows ID
  if( !_loadDesktopWindow() ) return;
  if( !root_pixmap_atom_ ) return;
  
  // load display 
  Display* display( QX11Info::display() );
    
  int format( 0 );
  unsigned long length( 0 ), after( 0 );
  unsigned char *data( 0 );
  Atom type( None );
  XGetWindowProperty( 
      display, desktop_, root_pixmap_atom_, 0L, 1L, false, AnyPropertyType,
      &type, &format, &length, &after, &data);
  
  Debug::Throw() << "BackgroundPixmap::reload - XGetWindowProperty" << endl; 
  
  // check property type
  if( type != XA_PIXMAP ) 
  {
    Debug::Throw() << "BackgroundPixmap::reload - invalid type" << endl; 
    return;
  }
  
  // try convert to a Pixmap
  Pixmap *pixmap( (Pixmap*) data );
  if( !pixmap ) 
  {
    Debug::Throw() << "BackgroundPixmap::reload - invalid pixmap" << endl; 
    return;
  }
  
  // retrieve Pixmap geometry
  Window root( None );
  int x( 0 ), y( 0 ); 
  unsigned int width( 0 ), height( 0 );
  unsigned int border( 0 );
  unsigned int depth( 0 );
  XGetGeometry( display, *pixmap, &root, &x, &y, &width, &height, &border, &depth );
  
  if( width && height )
  {
    Debug::Throw() << "BackgroundPixmap::reload - size: (" << width << "," << height << ")" << endl; 
  
    /* 
      convert to an XImage
      the ZPixmap format is a guess
      it's the only one which worked
    */
    XImage *x_image( XGetImage( display, *pixmap, x, y, width, height, AllPlanes, ZPixmap ) );
    PixelConverter converter( _converter( x_image ) );
    
    // convert to a QImage
    QImage image(width, height, QImage::Format_RGB32 );
    //image.fill( 0 );
    for( register unsigned int y = 0; y<height; y++ )
    {
      for( register unsigned int x = 0; x<width; x++ )
      { 
        image.setPixel( x, y, (*converter)( x_image, x, y ) ); 
      }
    } 
    
    background_pixmap_ = QPixmap::fromImage( image );
    Debug::Throw() << "BackgroundPixmap::reload - pixmap done." << endl; 
    
  } 
  
  #endif 
  
  if( background_pixmap_.isNull() )
  {
    Debug::Throw() << "BackgroundPixmap::reload - pixmap is null" << endl; 
    background_pixmap_ = QPixmap( qApp->desktop()->size() );
    background_pixmap_.fill();
  }
  
  emit backgroundChanged();
  
  return;
}

//_____________________________________________________
bool BackgroundPixmap::_loadDesktopWindow( void )
{

  #ifdef Q_WS_X11
  Debug::Throw( "BackgroundPixmap::_loadDesktopWindow.\n" );
  
  // get the display
  Display* display( QX11Info::display() );
  
  // if desktop was already loaded and isn't root remove XEvent forwarding
  if( desktop_ ) XSelectInput( display, desktop_, 0 );
  root_pixmap_atom_ = XInternAtom( display, "_XROOTPMAP_ID", true );
  if( !root_pixmap_atom_ ) return false;
  
  // retrieve desktop widget
  Window top( qApp->desktop()->winId() );
  Window parent( None );
  for( Window current_window = top; current_window != None; current_window = parent )
  {
  
    Window root( None );
    Window *children( 0 );
    unsigned int n_children( 0 );

    // load window tree
    if( !XQueryTree( display, current_window, &root, &parent, &children, &n_children) ) 
    return false;
    
    // try load atoms
    int format( 0 );
    unsigned long length( 0 ), after( 0 );
    unsigned char *data( 0 );
    Atom type;
    
    XGetWindowProperty( 
      display, current_window, root_pixmap_atom_, 0L, 1L, false, AnyPropertyType,
      &type, &format, &length, &after, &data);
  
    // check data
    if( type ) {
      desktop_ = current_window;
      XSelectInput( display, desktop_, PropertyChangeMask );
      Debug::Throw( "BackgroundPixmap::_LoadDesktopWindow - success.\n" );
      return true;
    }
    
  }
  #endif
  
  return false;

}

//_____________________________________________________
int BackgroundPixmap::_lowestBit( const unsigned int& val) 
{ 
    int i;
    int max_bits = sizeof(unsigned) * 8;
    unsigned test = 1;

    for( i=0; ( !(val & test)) && i < max_bits; i++, test<<=1);
    return (i == max_bits ) ? -1 : i;
}
 
#ifdef Q_WS_X11

//_____________________________________________________
BackgroundPixmap::PixelConverter BackgroundPixmap::_converter( XImage* image )
{
  
  // get RGB masks
  int red_shift(0);
  int green_shift(0);
  int blue_shift(0); 
  if( !(image->red_mask && image->green_mask && image->blue_mask) ) 
  {
    Visual *visual = (Visual*) QX11Info::appVisual();

    red_shift = _lowestBit( visual->red_mask);
    green_shift = _lowestBit( visual->green_mask);
    blue_shift  = _lowestBit( visual->blue_mask);
  
  } else {
    red_shift   = _lowestBit( image->red_mask);
    green_shift = _lowestBit( image->green_mask);
    blue_shift  = _lowestBit( image->blue_mask);
  }
  
  int bits_per_pixel = image->bits_per_pixel;
  Debug::Throw() << "_converter -"
      << " bits_per_pixel: " << bits_per_pixel
      << " red_shift: " << red_shift 
      << " green_shift: " << green_shift 
      << " blue_shift: " << blue_shift 
      << endl;
  
  // select converter
  if( bits_per_pixel == 32 ) 
  {
    Debug::Throw() << "BackgroundPixmap::_converter - _convertPixel_BIT32" << endl;
    return &_convertPixel_BIT32;
  }
  
  if( (bits_per_pixel == 16) && (red_shift == 11) && (green_shift == 5) && (blue_shift == 0)) 
  {
    Debug::Throw() << "BackgroundPixmap::_converter - _convertPixel_BIT16_RGB_565" << endl;
    return &_convertPixel_BIT16_RGB_565;
  }
  
  if( (bits_per_pixel == 16) && (red_shift == 10) && (green_shift == 5) && (blue_shift == 0)) 
  {
    Debug::Throw() << "BackgroundPixmap::_converter - _convertPixel_BIT16_RGB_555" << endl;
    return &_convertPixel_BIT16_RGB_555;
  }
  
  if( bits_per_pixel == 8 ) 
  {
    Debug::Throw() << "BackgroundPixmap::_converter - _convertPixel_BIT8" << endl;
    return &_convertPixel_BIT8;
  }
  
 
  Debug::Throw() << "BackgroundPixmap::_converter - _convertPixel_UNKNOWN" << endl;
  return &_convertPixel_UNKNOWN;
  
}

//_____________________________________________________
QRgb BackgroundPixmap::_convertPixel_BIT32( XImage* image, const unsigned int& x, const unsigned int& y )
{ return XGetPixel( image, x, y ); }

//_____________________________________________________
QRgb BackgroundPixmap::_convertPixel_BIT16_RGB_565( XImage* image, const unsigned int& x, const unsigned int& y )
{ 
  unsigned long pixel( XGetPixel( image, x, y ) );
  return ((pixel & 0xf800) << 8) | ((pixel & 0x7e0) << 5) | ((pixel & 0x1f) << 3); 
}

//_____________________________________________________
QRgb BackgroundPixmap::_convertPixel_BIT16_RGB_555( XImage* image, const unsigned int& x, const unsigned int& y )
{ 
  unsigned long pixel( XGetPixel( image, x, y ) );
  return ((pixel & 0x7c00) << 9) | ((pixel & 0x3e0) << 6) | ((pixel & 0x1f) << 3); 
}

//_____________________________________________________
QRgb BackgroundPixmap::_convertPixel_BIT8( XImage* image, const unsigned int& x, const unsigned int& y )
{ return XGetPixel( image, x, y ); }

//_____________________________________________________
QRgb BackgroundPixmap::_convertPixel_UNKNOWN( XImage* image, const unsigned int& x, const unsigned int& y )
{ return 0; }

#endif
