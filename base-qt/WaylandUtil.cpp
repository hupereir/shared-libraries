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

#include "WaylandUtil.h"
#include "CppUtil.h"

#include <QList>

#if WITH_XCB

#include <QWindow>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>

#include <QX11Info>

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#endif

//________________________________________________________________________
class WaylandUtil::Private
{

    public:

    //* constructor
    Private();

    //* true if running wayland
    bool isWayland_ = false;
    
};

//________________________________________________________________________
WaylandUtil::Private::Private()
{
    const auto platformName = QGuiApplication::platformName();
    isWayland_ = platformName.startsWith(QLatin1String("wayland"), Qt::CaseInsensitive);
}

//________________________________________________________________________
WaylandUtil& WaylandUtil::get()
{
    static WaylandUtil singleton;
    return singleton;
}

//________________________________________________________________________
WaylandUtil::WaylandUtil():
    d( new Private )
    {}

//________________________________________________________________________
bool WaylandUtil::isWayland()
{ return get().d->isWayland_; }
