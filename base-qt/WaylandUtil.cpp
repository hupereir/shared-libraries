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

    //* move a top level widget to a given position
    void moveWidget( QWidget*, const QPoint& );
   
    //* hide from taskbar
    void toggleHideWidgetFromTaskbar( QWidget*, bool );

    //* show on all desktops
    void toggleShowWidgetOnAllDesktops( QWidget*, bool );

    //* stay on top
    void toggleWidgetStaysOnTop( QWidget*, bool );

    private:
    
    #if WITH_KWAYLAND
    
    // return shell associated to a given widget
    KWayland::Client::PlasmaShell* _getShell( QWidget* );
    
    // return shell surface associated to a given widget
    KWayland::Client::PlasmaShellSurface* _getSurface( QWidget* );
    
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

#if WITH_KWAYLAND
//________________________________________________________________________
KWayland::Client::PlasmaShell* WaylandUtil::Private::_getShell( QWidget* w )
{
    if( !m_isWayland ) return nullptr;
    
    const auto iter = m_plasmaShellMap.find(w);
    if( iter != m_plasmaShellMap.end() )
    { return iter.value(); }
    
    using namespace KWayland::Client;
    
    auto connection = ConnectionThread::fromApplication(w);
    if (!connection) return nullptr;

    auto registry = new Registry(w);
    registry->create(connection);

    KWayland::Client::PlasmaShell* shell;
    
    QObject::connect(registry, &Registry::interfacesAnnounced, w, [&shell, registry, w] 
    {
        const auto interface = registry->interface(Registry::Interface::PlasmaShell);
        if (interface.name != 0) 
        { shell = registry->createPlasmaShell(interface.name, interface.version, w); }
    });

    registry->setup();
    connection->roundtrip();
    
    if( shell ) 
    { 
        m_plasmaShellMap.insert( w, shell ); 
        QObject::connect( w, &QObject::destroyed, [this,w]()
        {
            delete m_plasmaShellMap[w];
            m_plasmaShellMap.remove( w ); 
        } );
    }
    
    return shell;
    
}

//________________________________________________________________________
KWayland::Client::PlasmaShellSurface* WaylandUtil::Private::_getSurface( QWidget* w )
{
    if( !m_isWayland ) return nullptr;
    
    // check if surface already exists
    const auto iter = m_plasmaSurfaceMap.find(w);
    if( iter != m_plasmaSurfaceMap.end() )
    { return iter.value(); }
    
    auto shell = _getShell( w );
    if( !shell ) return nullptr;

    using namespace KWayland::Client;
    if (auto surface = Surface::fromWindow(w->windowHandle())) 
    {
        auto shell_surface = shell->createSurface(surface, w);
        shell_surface->setRole( PlasmaShellSurface::Role::Normal );
        shell_surface->setPanelBehavior( PlasmaShellSurface::PanelBehavior::WindowsCanCover );
        m_plasmaSurfaceMap.insert(w, shell_surface);
        QObject::connect( w, &QObject::destroyed, [this,w](){ 
            delete m_plasmaSurfaceMap[w];
            m_plasmaSurfaceMap.remove( w ); 
        } );
        
        return shell_surface;
    } else { 
        return nullptr;
    }    
}

#endif

//________________________________________________________________________
void WaylandUtil::Private::moveWidget( QWidget* w, const QPoint& position )
{    
    // get surface
    #if WITH_KWAYLAND
    if( const auto surface = _getSurface( w ) )
    { surface->setPosition( position ); }
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::toggleHideWidgetFromTaskbar( QWidget* w, bool value )
{    
    // get surface
    #if WITH_KWAYLAND
    if( const auto surface = _getSurface( w ) )
    { 
        surface->setSkipTaskbar(value);
        surface->setSkipSwitcher(value);       
    }
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::toggleShowWidgetOnAllDesktops( QWidget* w, bool value )
{    
    // get surface
    #if WITH_KWAYLAND
    using namespace KWayland::Client;
    if( const auto surface = _getSurface( w ) )
    { 
        if( value ) surface->setRole( PlasmaShellSurface::Role::Panel );
        else surface->setRole( PlasmaShellSurface::Role::Normal );
    }
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::toggleWidgetStaysOnTop( QWidget* w, bool value )
{    
    // get surface
    #if WITH_KWAYLAND
    using namespace KWayland::Client;
    if( const auto surface = _getSurface( w ) )
    { 
        Debug::Throw(0) << "WaylandUtil::Private::toggleWidgetStaysOnTop - value: " << value << Qt::endl;
        if( value ) surface->setPanelBehavior( PlasmaShellSurface::PanelBehavior::AlwaysVisible );
        else surface->setPanelBehavior( PlasmaShellSurface::PanelBehavior::WindowsCanCover );
    }
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

//________________________________________________________________________
void WaylandUtil::toggleHideWidgetFromTaskbar( QWidget* w, bool value )
{ get().d->toggleHideWidgetFromTaskbar( w, value ); }

//________________________________________________________________________
void WaylandUtil::toggleShowWidgetOnAllDesktops( QWidget* w, bool value )
{ get().d->toggleShowWidgetOnAllDesktops( w, value ); }

//________________________________________________________________________
void WaylandUtil::toggleWidgetStaysOnTop( QWidget* w, bool value )
{ get().d->toggleWidgetStaysOnTop( w, value ); }
