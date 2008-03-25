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

#include <qglobal.h>

#ifdef Q_WS_X11
#include <QWidget>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

namespace MAIN
{
    
  //! return true if property is found in NET_WM_STATE
  bool hasProperty( QWidget* widget, const Atom& atom );
  
  //! remove property from NET_WM_STATE
  void removeProperty( QWidget* widget, const Atom& atom );
  
};

#endif
#endif
