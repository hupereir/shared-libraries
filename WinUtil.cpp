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
#include "Debug.h"

#include <QLibrary>

#if defined(Q_OS_WIN)
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#include <windows.h>
#endif

//_______________________________________
class WinUtilPrivate
{

    public:

    //! constructor
    WinUtilPrivate( void )
    {
        #if defined(Q_OS_WIN)

        // define function pointer and try resolve from library
        dwmapi_ = QLibrary("dwmapi");
        blurBehindFunction_ =  (PtrDwmEnableBlurBehindWindow) dwmapi.resolve( "DwmEnableBlurBehindWindow" );
        #endif

    }

    //! destructor
    virtual ~WinUtilPrivate( void )
    {

        #if defined(Q_OS_WIN)
        // unlooad library
        dwmapi.unload();
        #endif

    }

    #if defined(Q_OS_WIN)

    //! blur behind structure
    typedef struct _DWM_BLURBEHIND {
        DWORD dwFlags;
        BOOL  fEnable;
        HRGN  hRgnBlur;
        BOOL  fTransitionOnMaximized;
    } DWM_BLURBEHIND;

    //! blur behind function
    typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND* );
    PtrDwmEnableBlurBehindWindow blurBehindFunction_;

    private:

    //! library
    QLibrary dwmapi_;
    #endif

};

//_______________________________________
WinUtil::WinUtil( QWidget* target ):
    private_( new WinUtilPrivate() ),
    target_( target )
{ Q_CHECK_PTR( target_ ); }

//_______________________________________
WinUtil::~WinUtil( void )
{ delete private_; }

//_______________________________________
void WinUtil::update( const QPixmap& pixmap, double opacity ) const
{

    #if defined(Q_OS_WIN)

    static bool first = true;
    if( first )
    {
        first = false;
        enableBlurBehind();
    }

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
    hBitmap = pixmap.toWinHBITMAP(QPixmap::PremultipliedAlpha);
    oldBitmap = (HBITMAP)SelectObject(memDc, hBitmap);

    UpdateLayeredWindow( target_->winId(), screenDc,  &topPos,  &size, memDc,  &pointSource, 0, &blend, ULW_ALPHA);

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
void WinUtil::enableBlurBehind( const BASE::Margins& ) const
{

    #if defined(Q_OS_WIN)

    // do nothing if windows version is too old
    if(QSysInfo::WindowsVersion < QSysInfo::WV_6_0) return;

    Debug::Throw(0) << "WinUtil::enableBlurBehind" << endl;
    if( !private_->blurBehindFunction_ ) return;

    // Create and populate the blur-behind structure.
    enum {DWM_BB_ENABLE=1};
    WinUtilPrivate::DWM_BLURBEHIND bb = {0};

    // Specify blur-behind and blur region.
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = true;
    bb.hRgnBlur = NULL;
    private_->blurBehindFunction(target_->winId(), &bb);

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
    return GetWindowLong( target_->winId(), GWL_EXSTYLE) & flag;
    #else
    return false;
    #endif

}

//_______________________________________
void WinUtil::setFlag( long int flag, bool value ) const
{

    #if defined(Q_OS_WIN)
    if( value ) SetWindowLong( target_->winId(), GWL_EXSTYLE, GetWindowLong( target_->winId(), GWL_EXSTYLE) | flag);
    else SetWindowLong( target_->winId(), GWL_EXSTYLE, GetWindowLong( target_->winId(), GWL_EXSTYLE) & (~flag));
    #endif


}
