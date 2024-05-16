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
#include "Debug.h"

#include <QGuiApplication>
#include <QMap>

#if WITH_KWAYLAND
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmashell.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/surface.h>
#endif

//________________________________________________________________________
class WaylandUtil::Private
{

    public:

    //* constructor
    Private();

    //* true if running wayland
    bool m_isWayland = false;

    //* inialize wayland for a given widdget
    void initWayland( QWidget* );
    
    //* initialize wayland surface for a given widget
    void initWaylandSurface( QWidget* );
 
    //* move a top level widget to a given position
    void moveWidget( QWidget*, const QPoint& );
   
    private:
    
    #if WITH_KWAYLAND
    // map widget to shell
    QMap<QWidget*, KWayland::Client::PlasmaShell*> m_plasmaShellMap;

    // map widget to surface
    QMap<QWidget*, KWayland::Client::PlasmaShellSurface*> m_plasmaSurfaceMap;
    #endif
    
};

//________________________________________________________________________
WaylandUtil::Private::Private()
{
    const auto platformName = QGuiApplication::platformName();
    m_isWayland = platformName.startsWith(QLatin1String("wayland"), Qt::CaseInsensitive);
}

//________________________________________________________________________
void WaylandUtil::Private::initWayland( QWidget* w )
{
    if( !m_isWayland ) return;
    #if WITH_KWAYLAND
    
    const auto iter = m_plasmaShellMap.find(w);
    if( iter != m_plasmaShellMap.end() )
    { return; }
    
    using namespace KWayland::Client;
    
    auto connection = ConnectionThread::fromApplication(w);
    if (!connection) return;

    auto registry = new Registry(w);
    registry->create(connection);

    QObject::connect(registry, &Registry::interfacesAnnounced, w, [this, registry, w] 
    {
        const auto interface = registry->interface(Registry::Interface::PlasmaShell);
        if (interface.name != 0) 
        {
            auto shell = registry->createPlasmaShell(interface.name, interface.version, w);
            if( shell ) { m_plasmaShellMap.insert(w, shell); }
        }
    });

    registry->setup();
    connection->roundtrip();
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::initWaylandSurface( QWidget* w )
{
    if( !m_isWayland ) return;
    #if WITH_KWAYLAND

    // check if surface already exists
    const auto surfaceIter = m_plasmaSurfaceMap.find(w);
    if( surfaceIter != m_plasmaSurfaceMap.end() ) return;
    
    // get shell
    const auto iter = m_plasmaShellMap.find(w);
    if( iter == m_plasmaShellMap.end() ) return;
    
    auto shell = iter.value();
    if (auto surface = KWayland::Client::Surface::fromWindow(w->windowHandle())) 
    {
        auto shell_surface = shell->createSurface(surface, w);
        shell_surface->setSkipTaskbar(true);
        shell_surface->setSkipSwitcher(true);       
        m_plasmaSurfaceMap.insert(w, shell_surface);
    }
    #endif    
}

//________________________________________________________________________
void WaylandUtil::Private::moveWidget( QWidget* w, const QPoint& position )
{
    // initialize everything
    initWayland(w);
    initWaylandSurface(w);
    
    // get surface
    #if WITH_KWAYLAND
    const auto surfaceIter = m_plasmaSurfaceMap.find(w);
    if( surfaceIter == m_plasmaSurfaceMap.end() ) return;
    surfaceIter.value()->setPosition( position );
    #endif
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
{ return get().d->m_isWayland; }

//________________________________________________________________________
void WaylandUtil::moveWidget( QWidget* w, const QPoint& position )
{ get().d->moveWidget( w, position ); }

