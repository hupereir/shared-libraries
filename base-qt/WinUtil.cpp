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
#include <QOperatingSystemVersion>

#if defined(Q_OS_WIN)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <QtWinExtras>

#include <windows.h>
#endif

//_______________________________________
class WinUtilPrivate final
{

    public:

    //* constructor
    explicit WinUtilPrivate()
    {
        #if defined(Q_OS_WIN)
        // define function pointer and try resolve from library
        dwmapi_.setFileName("dwmapi");
        dwmapi_.load();
        blurBehindFunction_ =  (PtrDwmEnableBlurBehindWindow) dwmapi_.resolve( "DwmEnableBlurBehindWindow" );
        #endif
    }

    //* destructor
    ~WinUtilPrivate()
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

#if defined(Q_OS_WIN)
//_______________________________________
WinUtil::WinUtil( QWidget* target )
    : target_( target )
{}
#else
WinUtil::WinUtil( QWidget*  )
{}
#endif

//_______________________________________
WinUtil::~WinUtil() = default;

//_______________________________________
void WinUtil::makeTransparent( double opacity ) const
{
    #if defined(Q_OS_WIN)
    if( !hasFlag( WS_EX_LAYERED) ) { setFlag( WS_EX_LAYERED, true ); }

    auto hwnd = HWND( target_->winId());

    // SetLayeredWindowAttributes(hwnd, 0, 255*opacity, LWA_ALPHA);
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, static_cast<BYTE>(opacity*255), AC_SRC_ALPHA };
    UpdateLayeredWindow( hwnd, nullptr,  nullptr,  nullptr, nullptr,  nullptr, 0, &blend, ULW_ALPHA);

    // repaint
    // RedrawWindow(hwnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
    #else
    Q_UNUSED( opacity );
    #endif

}

//_______________________________________
void WinUtil::update( const QPixmap& pixmap, double opacity ) const
{

    #if defined(Q_OS_WIN)
    if( !hasFlag( WS_EX_LAYERED) ) { setFlag( WS_EX_LAYERED, true ); }

    SIZE size = { pixmap.width(), pixmap.height() };
    POINT pointSource = {0,0};
    POINT topPos = { target_->x(),  target_->y() };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, static_cast<BYTE>(opacity*255), AC_SRC_ALPHA };

    HBITMAP hBitmap = QtWin::toHBITMAP(pixmap, QtWin::HBitmapPremultipliedAlpha);

    auto screenDc = GetDC(nullptr);
    auto memDc = CreateCompatibleDC(screenDc);

    HBITMAP oldBitmap =static_cast<HBITMAP>( SelectObject(memDc, hBitmap) );

    UpdateLayeredWindow( HWND(target_->winId()), screenDc,  &topPos,  &size, memDc,  &pointSource, 0, &blend, ULW_ALPHA);

    ReleaseDC( nullptr, screenDc);
    if (hBitmap != nullptr)
    {
        SelectObject(memDc, oldBitmap);
        DeleteObject(hBitmap);
    }
    DeleteDC(memDc);
    #else
    Q_UNUSED( opacity );
    Q_UNUSED( pixmap );
    #endif
}

//_______________________________________
void WinUtil::enableBlurBehind( Base::Margins margins )
{

    #if defined(Q_OS_WIN)
    // do nothing if windows version is too old
    if( QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows7 ) return;

    // initialize private
    if( !private_ ) private_.reset( new WinUtilPrivate );

    // check function pointer
    if( !private_->blurBehindFunction_ ) return;

    // create structure
    WinUtilPrivate::DWM_BLURBEHIND blurBehind;

    // create blur region
    const QRect rect( margins.adjustedRect( target_->rect() ) );
    auto region = CreateRectRgn( rect.left(), rect.top(), rect.right(), rect.bottom() );

    // create flags
    blurBehind.dwFlags = WinUtilPrivate::DWM_BB_ENABLE|WinUtilPrivate::DWM_BB_BLURREGION;
    blurBehind.fEnable = true;
    blurBehind.hRgnBlur = region;

    // assign blur to window
    private_->blurBehindFunction_( HWND(target_->winId()), &blurBehind);
    #else
    Q_UNUSED( margins );
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
    } else {
        return false;
    }
    #else
    Q_UNUSED( state );
    return false;
    #endif
}

//_______________________________________
bool WinUtil::hasFlag( long int flag ) const
{

    #if defined(Q_OS_WIN)
    return GetWindowLong( HWND(target_->winId()), GWL_EXSTYLE) & flag;
    #else
    Q_UNUSED( flag );
    return false;
    #endif

}

//_______________________________________
void WinUtil::setFlag( long int flag, bool value ) const
{
    #if defined(Q_OS_WIN)
    if( value ) SetWindowLong( HWND(target_->winId()), GWL_EXSTYLE, GetWindowLong( HWND(target_->winId()), GWL_EXSTYLE) | flag);
    else SetWindowLong( HWND(target_->winId()), GWL_EXSTYLE, GetWindowLong( HWND(target_->winId()), GWL_EXSTYLE) & (~flag));
    #else
    Q_UNUSED( flag );
    Q_UNUSED( value );
    #endif
}
