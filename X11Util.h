#ifndef X11Util_h
#define X11Util_h

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

#include <QWidget>
#include <map>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

class X11Util
{
  
  public:
  
  //! Atoms enumeration
  enum Atoms
  {
    _NET_SUPPORTED,
    _NET_WM_STATE,
    _NET_WM_STATE_STICKY,
    _NET_WM_STATE_STAYS_ON_TOP,
    _NET_WM_STATE_ABOVE,
    _NET_WM_STATE_SKIP_TASKBAR,
    _NET_WM_MOVERESIZE
  };
  
  enum Direction
  {
    _NET_WM_MOVERESIZE_SIZE_TOPLEFT=0,
    _NET_WM_MOVERESIZE_SIZE_TOP=1,
    _NET_WM_MOVERESIZE_SIZE_TOPRIGHT=2,
    _NET_WM_MOVERESIZE_SIZE_RIGHT=3,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT=4,
    _NET_WM_MOVERESIZE_SIZE_BOTTOM=5,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT=6,
    _NET_WM_MOVERESIZE_SIZE_LEFT=7,
    _NET_WM_MOVERESIZE_MOVE=8,
    _NET_WM_MOVERESIZE_SIZE_KEYBOARD=9,
    _NET_WM_MOVERESIZE_MOVE_KEYBOARD=10,    
  };
  
  //! supported atoms
  static bool isSupported( const Atoms& atom );

  //! return true if property is found in NET_WM_STATE
  static bool hasProperty( const QWidget& widget, const Atoms& atom );
  
  //! change property
  static bool changeProperty( const QWidget& widget, const Atoms& atom, const unsigned int& value );
  
  //! remove property from NET_WM_STATE
  static bool removeProperty( const QWidget& widget, const Atoms& atom );
  
  //! move widget using X11 window manager
  /*! returns true on success */
  static bool moveWidget( const QWidget& widget, const QPoint& position )
  { return moveResizeWidget( widget, position, X11Util::_NET_WM_MOVERESIZE_MOVE ); }

  //! move/resize widget using X11 window manager
  /*! returns true on success */
  static bool moveResizeWidget( const QWidget& widget, const QPoint& position, const Direction& direction );
  
  
  #ifdef Q_WS_X11
  
  //! find atom
  static Atom findAtom( const Atoms& atom );
  
  #endif
  private:
  
  //! atom names
  typedef std::map<Atoms, std::string> AtomNameMap; 
  
  //! atom names
  static AtomNameMap _initializeAtomNames( void );
  
  //! atom names
  static AtomNameMap  atom_names_;
  
  //! supported atoms
  typedef std::map<Atoms, bool > SupportedAtomMap;
  
  //! supported atoms
  static SupportedAtomMap supported_atoms_;
  
  #ifdef Q_WS_X11
  
  //! atom map
  typedef std::map<Atoms, Atom> AtomMap;
  
  //! atoms
  static AtomMap atoms_;
  
  #endif
  
};

#endif
