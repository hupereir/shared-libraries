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
      
    //* store relevant wayland information for a given widget
    class WidgetInformation
    {
        public:
        
        //* initialize wayland
        void initWayland( QWidget* );
        
        //* global position
        QPoint m_position;
        
        #if WITH_KWAYLAND
        KWayland::Client::PlasmaShell* m_plasmaShell = nullptr;
        KWayland::Client::PlasmaShellSurface* m_plasmaShellSurface = nullptr;
        #endif
        
    };

    //* get widget information for a given widget
    WidgetInformation& _getWidgetInformation( QWidget* );
    
    #if WITH_KWAYLAND
    
    //* return shell associated to a given widget
    KWayland::Client::PlasmaShell* _getShell( QWidget* );
    
    //* return shell surface associated to a given widget
    KWayland::Client::PlasmaShellSurface* _getSurface( QWidget* );
    
    #endif
  
    //* map widget to widget information
    QMap<QWidget*, WidgetInformation> m_widgetInformationMap;
    
};

//________________________________________________________________________
WaylandUtil::Private::Private()
{
    const auto platformName = QGuiApplication::platformName();
    m_isWayland = platformName.startsWith(QLatin1String("wayland"), Qt::CaseInsensitive);
}

//________________________________________________________________________
WaylandUtil::Private::WidgetInformation& WaylandUtil::Private::_getWidgetInformation( QWidget* w )
{
    auto iter = m_widgetInformationMap.find( w );
    if( iter == m_widgetInformationMap.end() ) 
    {
        iter = m_widgetInformationMap.insert( w, WidgetInformation() );
        iter.value().initWayland(w);
        QObject::connect( w, &QObject::destroyed, [this,w]() { m_widgetInformationMap.remove( w ); } );
    }
    
    return iter.value();
}

//________________________________________________________________________
void WaylandUtil::Private::WidgetInformation::initWayland( QWidget* w )
{
    #if WITH_KWAYLAND

    // initialize shell
    if( !m_plasmaShell )
    {
        using namespace KWayland::Client;
        
        // get connection and check
        auto connection = ConnectionThread::fromApplication(w);
        if (!connection) return;
 
        // create registry
        auto registry = new Registry(w);
        registry->create(connection);
   
        QObject::connect(registry, &Registry::interfacesAnnounced, w, [this, registry, w] 
        {
            const auto interface = registry->interface(Registry::Interface::PlasmaShell);
            if (interface.name != 0) 
            { m_plasmaShell = registry->createPlasmaShell(interface.name, interface.version, w); }
        });
 
        registry->setup();
        connection->roundtrip();
        if( m_plasmaShell )
        { QObject::connect( w, &QObject::destroyed, [this]() { delete m_plasmaShell; } ); }

    }
    
    // initialize shell surface
    if( !m_plasmaShellSurface && m_plasmaShell )
    {
        using namespace KWayland::Client;
        if (auto surface = Surface::fromWindow(w->windowHandle())) 
        {
            m_plasmaShellSurface = m_plasmaShell->createSurface(surface, w);
            m_plasmaShellSurface->setRole( PlasmaShellSurface::Role::Normal );
            m_plasmaShellSurface->setPanelBehavior( PlasmaShellSurface::PanelBehavior::WindowsCanCover );
            QObject::connect( w, &QObject::destroyed, [this]() { delete m_plasmaShellSurface; } );
        }
    }
    
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::moveWidget( QWidget* w, const QPoint& position )
{    
    if( !isWayland() ) return;
    
    #if WITH_KWAYLAND        
    auto&& winfo = _getWidgetInformation(w);
    winfo.m_position = position;
    if( winfo.m_plasmaShellSurface ) 
    { winfo.m_plasmaShellSurface->setPosition( position ); }
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::toggleHideWidgetFromTaskbar( QWidget* w, bool value )
{    
    if( !isWayland() ) return;

    #if WITH_KWAYLAND
    auto&& winfo = _getWidgetInformation(w);
    if( winfo.m_plasmaShellSurface ) 
    { 
        winfo.m_plasmaShellSurface->setSkipTaskbar(value);
        winfo.m_plasmaShellSurface->setSkipSwitcher(value);       
    }
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::toggleShowWidgetOnAllDesktops( QWidget* w, bool value )
{    
    // get surface
    #if WITH_KWAYLAND
    using namespace KWayland::Client;
    auto&& winfo = _getWidgetInformation(w);
    if( winfo.m_plasmaShellSurface ) 
    { 
        if( value ) winfo.m_plasmaShellSurface->setRole( PlasmaShellSurface::Role::Panel );
        else winfo.m_plasmaShellSurface->setRole( PlasmaShellSurface::Role::Normal );
    }
    #endif
}

//________________________________________________________________________
void WaylandUtil::Private::toggleWidgetStaysOnTop( QWidget* w, bool value )
{    
    // get surface
    #if WITH_KWAYLAND
    using namespace KWayland::Client;
    auto&& winfo = _getWidgetInformation(w);
    if( winfo.m_plasmaShellSurface ) 
    { 
        if( value ) winfo.m_plasmaShellSurface->setPanelBehavior( PlasmaShellSurface::PanelBehavior::AlwaysVisible );
        else winfo.m_plasmaShellSurface->setPanelBehavior( PlasmaShellSurface::PanelBehavior::WindowsCanCover );
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
