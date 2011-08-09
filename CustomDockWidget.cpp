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

#include "CustomDockWidget.h"
#include "BaseMainWindow.h"
#include "Debug.h"
#include "Singleton.h"

//_______________________________________________________________
CustomDockWidget::AreaMap& CustomDockWidget::_dockWidgetAreas( void )
{
    static AreaMap areas = _initializeAreas();
    return areas;
}

//______________________________________________________________
CustomDockWidget::CustomDockWidget( const QString& title, QWidget* parent, const QString& optionName ):
    QDockWidget( title, parent ),
    Counter( "CustomDockWidget" ),
    optionName_( optionName ),
    title_( 0L ),
    locked_( false )
{
    Debug::Throw( "CustomDockWidget::CustomDockWidget.\n" );

    connect( this, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ), SLOT( _updateLocation( Qt::DockWidgetArea ) ) );
    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    _updateConfiguration();

}

//______________________________________________________________
void CustomDockWidget::setLocked( bool value )
{

    Debug::Throw() << "CustomDockWidget::setLocked: " << value << endl;
    if( value != locked_ )
    {

        locked_ = value;

        if( locked_ )
        {

            if( !title_ ) title_ = new TitleBar( this );
            setTitleBarWidget( title_ );

        } else setTitleBarWidget(0);

    }

}

//_______________________________________________________________
void CustomDockWidget::_updateConfiguration( void )
{
    Debug::Throw( "CustomDockWidget::_updateConfiguration.\n" );

    // lock
    BaseMainWindow* mainwindow( qobject_cast<BaseMainWindow*>( window() ) );
    if( mainwindow && mainwindow->hasOptionName() && XmlOptions::get().find( mainwindow->lockLayoutOptionName() ) )
    { setLocked( XmlOptions::get().get<bool>( mainwindow->lockLayoutOptionName() ) ); }

    if( hasPreferredArea() ) emit dockLocationChanged( preferredArea() );

}

//_______________________________________________________________
void CustomDockWidget::_updateLocation( Qt::DockWidgetArea area )
{

    if( !optionName_.isEmpty() )
    { XmlOptions::get().setRaw( _locationOptionName(), CustomDockWidget::areaToName( area ) ); }

}

//_______________________________________________________________
CustomDockWidget::AreaMap CustomDockWidget::_initializeAreas( void )
{
    AreaMap out;
    out.insert( std::make_pair( "none", Qt::NoDockWidgetArea ) );
    out.insert( std::make_pair( "left", Qt::LeftDockWidgetArea ) );
    out.insert( std::make_pair( "right", Qt::RightDockWidgetArea ) );
    out.insert( std::make_pair( "top", Qt::TopDockWidgetArea ) );
    out.insert( std::make_pair( "bottom", Qt::BottomDockWidgetArea ) );
    return out;
}
