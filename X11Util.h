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
    _NET_WM_STATE,
    _NET_WM_STATE_STICKY,
    _NET_WM_STATE_SKIP_TASKBAR
  };
  
  //! return true if property is found in NET_WM_STATE
  static bool hasProperty( QWidget* widget, const Atoms& atom );
  
  //! change property
  static void changeProperty( QWidget* widget, const Atoms& atom, const unsigned int& value );
  
  //! remove property from NET_WM_STATE
  static void removeProperty( QWidget* widget, const Atoms& atom );
  
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
  
  #ifdef Q_WS_X11
  
  //! atom map
  typedef std::map<Atoms, Atom> AtomMap;
  
  //! atoms
  static AtomMap atoms_;
  
  #endif
  
};

#endif
