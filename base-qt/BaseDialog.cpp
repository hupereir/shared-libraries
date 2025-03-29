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
#include "WindowManager.h"
#include "XcbUtil.h"
#include "XmlOptions.h"



#include <QTextStream>
#include <QWindowStateChangeEvent>

//__________________________________________________
BaseDialog::BaseDialog( QWidget* parent, Qt::WindowFlags flags ):
    QDialog( parent, flags ),
    monitor_( this ),
    iconSize_( IconSize::get( IconSize::VeryHuge ) )
{
    Debug::Throw( QStringLiteral("BaseDialog::BaseDialog.\n") );
    setSizeGripEnabled(false);

    WindowManager::get().registerWidget( this );
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
void BaseDialog::setWindowTitle( const QString& title )
{ QDialog::setWindowTitle( Util::windowTitle( title ) ); }

//________________________________________________________________
BaseDialog& BaseDialog::centerOnPointer()
{

    Debug::Throw( QStringLiteral("BaseDialog::centerOnPointer.\n") );
    move( QtUtil::centerOnPointer( sizeHint() ) );
    return *this;
}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnDesktop()
{

    Debug::Throw( QStringLiteral("BaseDialog::centerOnDesktop.\n") );
    move( QtUtil::centerOnDesktop( sizeHint() ) );
    return *this;

}

//________________________________________________________________
BaseDialog& BaseDialog::centerOnWidget( QWidget* parent )
{

    Debug::Throw( QStringLiteral("BaseDialog::centerOnWidget.\n") );
    move( QtUtil::centerOnWidget( sizeHint(), parent ) );
    return *this;

}

//________________________________________________________________
void BaseDialog::uniconify()
{
    if( isMinimized() )
    {
        if( wasMaximized_ ) showMaximized();
        else showNormal();
    } else if( isHidden() ) show();
    activateWindow();
    raise();
}

//__________________________________________________________
void BaseDialog::toggleSticky( bool state )
{
    #if WITH_XCB
    if( XcbUtil::get().isSupported( XcbDefines::AtomId::_NET_WM_STATE_STICKY ) )
    {
        XcbUtil::get().changeState( this, XcbDefines::AtomId::_NET_WM_STATE_STICKY, state );
    } else if( XcbUtil::get().isSupported( XcbDefines::AtomId::_NET_WM_DESKTOP ) ) {
        const auto desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), XcbDefines::AtomId::_NET_CURRENT_DESKTOP );
        XcbUtil::get().changeCardinal( this, XcbDefines::AtomId::_NET_WM_DESKTOP, state ? XcbDefines::ALL_DESKTOPS:desktop );
    }
    #else
    Q_UNUSED(state)
    #endif
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
