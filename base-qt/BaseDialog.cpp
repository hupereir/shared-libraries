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

#include "BaseDialog.h"
#include "Debug.h"
#include "IconSize.h"
#include "QtUtil.h"
#include "Util.h"
#include "XmlOptions.h"
#include "XcbUtil.h"

#include <QTextStream>
#include <QWindowStateChangeEvent>

//__________________________________________________
BaseDialog::BaseDialog( QWidget* parent, Qt::WindowFlags flags ):
    QDialog( parent, flags ),
    monitor_( this ),
    iconSize_( IconSize::get( IconSize::VeryHuge ) )
{
    Debug::Throw( "BaseDialog::BaseDialog.\n" );
    setSizeGripEnabled(false);
}

//__________________________________________________
QSize BaseDialog::minimumSizeHint() const
{
    auto&& out( monitor_.sizeHint() );
    return out.isValid() ? out:QDialog::minimumSizeHint();
}

//__________________________________________________
QSize BaseDialog::sizeHint() const
{
    auto&& out( monitor_.sizeHint() );
    return out.isValid() ? out:QDialog::sizeHint();
}

//__________________________________________________
BaseDialog& BaseDialog::setWindowTitle( const QString& title )
{

    Debug::Throw( "BaseDialog::setWindowTitle.\n" );
    QDialog::setWindowTitle( Util::windowTitle( title ) );
    return *this;

}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnPointer()
{

    Debug::Throw( "BaseDialog::centerOnPointer.\n" );
    move( QtUtil::centerOnPointer( sizeHint() ) );
    return *this;
}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnDesktop()
{

    Debug::Throw( "BaseDialog::centerOnDesktop.\n" );
    move( QtUtil::centerOnDesktop( sizeHint() ) );
    return *this;

}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnWidget( QWidget* parent )
{

    Debug::Throw( "BaseDialog::centerOnWidget.\n" );
    move( QtUtil::centerOnWidget( sizeHint(), parent ) );
    return *this;

}

//________________________________________________________________
BaseDialog& BaseDialog::uniconify()
{

    Debug::Throw( "BaseDialog::uniconify.\n" );
    if( isMinimized() )
    {

        if( wasMaximized_ ) showMaximized();
        else showNormal();

    } else if( isHidden() ) show();

    activateWindow();
    raise();
    return *this;

}

//__________________________________________________________
BaseDialog& BaseDialog::toggleSticky( bool state )
{

    Debug::Throw( "BaseDialog::toggleSticky.\n" );

    #if HAVE_XCB
    if( XcbUtil::get().isSupported( XcbDefines::_NET_WM_STATE_STICKY ) )
    {

        XcbUtil::get().changeState( this, XcbDefines::_NET_WM_STATE_STICKY, state );

    } else if( XcbUtil::get().isSupported( XcbDefines::_NET_WM_DESKTOP ) ) {

        unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), XcbDefines::_NET_CURRENT_DESKTOP );
        XcbUtil::get().changeCardinal( this, XcbDefines::_NET_WM_DESKTOP, state ? XcbDefines::ALL_DESKTOPS:desktop );

    }

    #endif

    return *this;
}

//_______________________________________________________
bool BaseDialog::event( QEvent* event )
{

    // check that all needed widgets/actions are valid and checked.
    switch (event->type())
    {

        case QEvent::WindowStateChange:
        {
            // cast
            QWindowStateChangeEvent *stateEvent( static_cast<QWindowStateChangeEvent*>(event) );

            if( windowState() & Qt::WindowMinimized )
            { _setWasMaximized( stateEvent->oldState() & Qt::WindowMaximized ); }

        }
        break;

        default: break;
    }

    return QDialog::event( event );

}
