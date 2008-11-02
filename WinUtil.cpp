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
  \file    WinUtil.cpp
  \brief   some Win32 specific utilities
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include "WinUtil.h"

#ifdef Q_WS_WIN
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#include <windows.h>
#endif

#include "Debug.h"

using namespace std;

//_______________________________________
void WinUtil::update( QPixmap& pixmap ) const
{

  #ifdef Q_WS_WIN
  if( !_isLayered() ) _setLayered();
  
  HBITMAP oldBitmap;
  HBITMAP hBitmap;	
  SIZE size;
  size.cx = pixmap.width();
  size.cy = pixmap.height();
  HDC screenDc = GetDC(NULL);
  POINT pointSource;
  pointSource.x = 0;
  pointSource.y = 0; 
  POINT topPos;
  topPos.x = _target().x();
  topPos.y = _target().y();	
  HDC memDc = CreateCompatibleDC(screenDc);
  BLENDFUNCTION blend;
  blend.BlendOp             = AC_SRC_OVER;
  blend.BlendFlags          = 0;
  blend.SourceConstantAlpha = 255;
  blend.AlphaFormat         = AC_SRC_ALPHA;
  hBitmap = pixmap.toWinHBITMAP(QPixmap::PremultipliedAlpha); 
  oldBitmap = (HBITMAP)SelectObject(memDc, hBitmap);
  
  UpdateLayeredWindow( _target().winId(), screenDc,  &topPos,  &size, memDc,  &pointSource, 0, &blend, ULW_ALPHA);
  
  ReleaseDC( NULL, screenDc);
  if (hBitmap != NULL)
  {
    SelectObject(memDc, oldBitmap);
    DeleteObject(hBitmap); 
    DeleteObject(hBitmap);
  }
  DeleteDC(memDc); 
  
  #endif
}

//_______________________________________
bool WinUtil::_isLayered( void ) const
{

  Debug::Throw( "WinUtil::isLayered" );
  #ifdef Q_WS_WIN
  return GetWindowLong( _target().winId(), GWL_EXSTYLE) & WS_EX_LAYERED;
  #else 
  return false;
  #endif
  
}

//_______________________________________
void WinUtil::_setLayered() const
{
  
  #ifdef Q_WS_WIN
  SetWindowLong( _target().winId(), GWL_EXSTYLE, GetWindowLong( _target().winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
  #endif
  
}
