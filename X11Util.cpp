
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
  \file    X11Util.h
  \brief   some X11 specific utilities
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include "Debug.h"
#include "X11Util.h"

#ifdef Q_WS_X11

//________________________________________________________________________
bool MAIN::hasProperty( QWidget* widget, const Atom& atom )
{
    Debug::Throw( "MAIN::hasProperty.\n" );

    Display* display( QX11Info::display() );
    Atom net_wm_state( XInternAtom (display, "_NET_WM_STATE", False) );

    Atom actual;
    int format;
    unsigned char *data;
    
    // try retrieve property
    unsigned long offset = 0;
    while( 1 )
    {
      unsigned long n, left;
      XGetWindowProperty( 
        display, widget->winId(), net_wm_state, 
        offset, 1L, false, 
        XA_ATOM, &actual,  
        &format, &n, &left, 
        (unsigned char **) &data);
      
      // finish if no data is found
      if( data == None ) break;

      // try cast data to atom and compare
      Atom found( *(Atom*)data );
      if( found == atom ) return true;
      
      // move forward if more data remains
      if( !left ) break;
      else offset ++;
      
    }
        
    // no match found
    return false;
    
}

//________________________________________________________________________
void MAIN::removeProperty( QWidget* widget, const Atom& atom )
{
  Debug::Throw( "MAIN::removeProperty.\n" );
  
  Display* display( QX11Info::display() );
  Atom net_wm_state( XInternAtom (display, "_NET_WM_STATE", False) );
  
  Atom actual;
  int format;
  unsigned char *data;
  
  // try retrieve property
  unsigned long offset = 0;
  std::list<Atom> atoms;
  while( 1 )
  {
    unsigned long n, left;
    XGetWindowProperty( 
      display, widget->winId(), net_wm_state, 
      offset, 1L, false, 
      XA_ATOM, &actual,  
      &format, &n, &left, 
      (unsigned char **) &data);
    
    if( data == None ) break;      
    
    // try cast data to atom
    Atom found( *(Atom*)data );
    if( found != atom ) atoms.push_back( found );
    if( !left ) break;
    else offset ++;
  }
  
  // delete property
  XDeleteProperty( display, widget->winId(), net_wm_state );
  
  // re-add atoms that are not the one to be deleted
  for( std::list<Atom>::iterator iter = atoms.begin(); iter != atoms.end(); iter++ )
  {XChangeProperty( display, widget->winId(), net_wm_state, XA_ATOM, 32, PropModeAppend, (unsigned char *)&*iter, 1); }
  
  return;
}

#endif
