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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "WinUtil.h"
#include "Debug.h"

#include <QLibrary>

#if defined(Q_OS_WIN)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif

#if QT_VERSION >= 0x050000
#include <QtWinExtras>
#endif

#include <windows.h>
#endif

//_______________________________________
class WinUtilPrivate
{

    public:

    //* constructor
    WinUtilPrivate( void )
    {
        #if defined(Q_OS_WIN)
        // define function pointer and try resolve from library
        dwmapi_ .setFileName("dwmapi");
        dwmapi_.load();
        blurBehindFunction_ =  (PtrDwmEnableBlurBehindWindow) dwmapi_.resolve( "DwmEnableBlurBehindWindow" );
        #endif
    }

    //* destructor
    virtual ~WinUtilPrivate( void )
    {

        #if defined(Q_OS_WIN)
        // unlooad library
        dwmapi_.unload();
        #endif

    }

    #if defined(Q_OS_WIN)

    //* blur flags
    enum
    {
        DWM_BB_ENABLE = 1<<0,
        DWM_BB_BLURREGION = 1<<1,
        DWM_BB_TRANSITIONONMAXIMIZED = 1<<2
    };

    //* blur behind structure
    struct DWM_BLURBEHIND
    {
        DWORD dwFlags;
        BOOL  fEnable;
        HRGN  hRgnBlur;
        BOOL  fTransitionOnMaximized;
    };

    //* blur behind function
    typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND* );
    PtrDwmEnableBlurBehindWindow blurBehindFunction_;

    private:

    //* library
    QLibrary dwmapi_;
    #endif

};

//_______________________________________
WinUtil::WinUtil( QWidget* target ):
    target_( target )
{ Q_CHECK_PTR( target_ ); }

//_______________________________________
WinUtil::~WinUtil( void )
{}

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
    topPos.x = target_->x();
    topPos.y = target_->y();
    HDC memDc = CreateCompatibleDC(screenDc);
    BLENDFUNCTION blend;
    blend.BlendOp             = AC_SRC_OVER;
    blend.BlendFlags          = 0;
    blend.SourceConstantAlpha = int( opacity*255 );
    blend.AlphaFormat         = AC_SRC_ALPHA;

    #if QT_VERSION >= 0x050000
    hBitmap = QtWin::toHBITMAP(pixmap, QtWin::HBitmapPremultipliedAlpha);
    #else
    hBitmap = pixmap.toWinHBITMAP(QPixmap::PremultipliedAlpha);
    #endif

    oldBitmap = (HBITMAP)SelectObject(memDc, hBitmap);

    UpdateLayeredWindow( HWND(target_->winId()), screenDc,  &topPos,  &size, memDc,  &pointSource, 0, &blend, ULW_ALPHA);

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
void WinUtil::enableBlurBehind( const Base::Margins& margins )
{

    #if defined(Q_OS_WIN)

    // do nothing if windows version is too old
    if(QSysInfo::WindowsVersion < QSysInfo::WV_6_0) return;

    // initialize private
    if( !private_ ) private_ = new WinUtilPrivate();

    // check function pointer
    if( !private_->blurBehindFunction_ ) return;

    // create structure
    WinUtilPrivate::DWM_BLURBEHIND blurBehind;

    // create blur region
    const QRect rect( margins.adjustedRect( target_->rect() ) );
    HRGN region = CreateRectRgn( rect.left(), rect.top(), rect.right(), rect.bottom() );

    // create flags
    blurBehind.dwFlags = WinUtilPrivate::DWM_BB_ENABLE|WinUtilPrivate::DWM_BB_BLURREGION;
    blurBehind.fEnable = true;
    blurBehind.hRgnBlur = region;

    // assign blur to window
    private_->blurBehindFunction_( HWND(target_->winId()), &blurBehind);

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
    return GetWindowLong( HWND(target_->winId()), GWL_EXSTYLE) & flag;
    #else
    return false;
    #endif

}

//_______________________________________
void WinUtil::setFlag( long int flag, bool value ) const
{

    #if defined(Q_OS_WIN)
    if( value ) SetWindowLong( HWND(target_->winId()), GWL_EXSTYLE, GetWindowLong( HWND(target_->winId()), GWL_EXSTYLE) | flag);
    else SetWindowLong( HWND(target_->winId()), GWL_EXSTYLE, GetWindowLong( HWND(target_->winId()), GWL_EXSTYLE) & (~flag));
    #endif

}
