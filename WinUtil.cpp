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

#include "WinUtil.h"

#if defined(Q_OS_WIN)
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#include <windows.h>
#endif

//_______________________________________
void WinUtil::update( const QPixmap& pixmap, double opacity ) const
{

    #if defined(Q_OS_WIN)
    if( !hasFlag( WS_EX_LAYERED) ) { setFlag( WS_EX_LAYERED, true ); }

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
    blend.SourceConstantAlpha = int( opacity*255 );
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
bool WinUtil::toggleHideFromTaskBar( bool state ) const
{

    #if defined(Q_OS_WIN)

    // check flag has changed
    if( hasFlag( WS_EX_TOOLWINDOW ) != state )
    {
        // hide window if visible
        bool wasVisible( target_->isVisible() );
        if( wasVisible ) target_->hide();

        // update flag
        setFlag( WS_EX_TOOLWINDOW, state );

        // show again
        if( wasVisible ) target_->show();
        return true;
    }

    #endif

    // do nothing and return false
    return false;

}

//_______________________________________
bool WinUtil::hasFlag( long int flag ) const
{

    #if defined(Q_OS_WIN)
    return GetWindowLong( _target().winId(), GWL_EXSTYLE) & flag;
    #else
    return false;
    #endif

}

//_______________________________________
void WinUtil::setFlag( long int flag, bool value ) const
{

    #if defined(Q_OS_WIN)
    if( value ) SetWindowLong( _target().winId(), GWL_EXSTYLE, GetWindowLong( _target().winId(), GWL_EXSTYLE) | flag);
    else SetWindowLong( _target().winId(), GWL_EXSTYLE, GetWindowLong( _target().winId(), GWL_EXSTYLE) & (~flag));
    #endif


}
