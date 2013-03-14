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

#include "BaseMainWindow.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "Singleton.h"
#include "XmlOptions.h"

//_______________________________________________________________
CustomToolBar::AreaMap& CustomToolBar::_toolbarAreas( void )
{
    static AreaMap areas = _initializeAreas();
    return areas;
}

//_______________________________________________________________
CustomToolBar::CustomToolBar( const QString& title, QWidget* parent, const QString& optionName ):
    QToolBar( title, parent ),
    Counter( "CustomToolBar" ),
    optionName_( optionName ),
    locationOptionName_( optionName + "_LOCATION" ),
    transparent_( false ),
    sizeFromOptions_( true ),
    lockFromOptions_( true ),
    appearsInMenu_( false )
{
    Debug::Throw( "CustomToolBar::CustomToolBar.\n" );

    // assign option name to object
    if( !optionName_.isEmpty() ) { setObjectName( optionName ); }

    _installActions();

    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    _updateConfiguration();
}

//_______________________________________________________________
CustomToolBar::~CustomToolBar( void )
{ Debug::Throw( "~CustomToolBar::CustomToolBar.\n" ); }

//_______________________________________________________________
void CustomToolBar::paintEvent( QPaintEvent* event )
{ if( !transparent_ ) return QToolBar::paintEvent( event ); }

//_______________________________________________________________
void CustomToolBar::showEvent( QShowEvent* event )
{
    Debug::Throw() << "CustomToolBar::showEvent - name: " << optionName_ << endl;
    if( !isHidden() ) visibilityAction().setChecked( true );
    QToolBar::showEvent(event);
}

//_______________________________________________________________
void CustomToolBar::hideEvent( QHideEvent* event )
{

    Debug::Throw() << "CustomToolBar::hideEvent - name: " << optionName_ << endl;
    if( isHidden() ) visibilityAction().setChecked( false );
    QToolBar::hideEvent(event);

}

//_______________________________________________________________
void CustomToolBar::moveEvent( QMoveEvent* event )
{
    if( isFloating() || optionName_.isEmpty() ) return QToolBar::moveEvent( event );

    QMainWindow* parent( qobject_cast<QMainWindow*>( parentWidget() ) );
    if( parent )
    { XmlOptions::get().set<int>( locationOptionName_, parent->toolBarArea( this ) ); }

}

//_______________________________________________________________
void CustomToolBar::_toggleVisibility( bool state )
{

    Debug::Throw() << "CustomToolBar::_toggleVisibility - name: " << optionName_ << " state: " << state << endl;
    if( !optionName_.isEmpty() )
    {

        // save state
        XmlOptions::get().set<bool>( optionName_, state );

        // save position
        // try cast parent to QMainWindow
        QMainWindow* parent( qobject_cast<QMainWindow*>( parentWidget() ) );
        if( parent )
        { XmlOptions::get().set<int>( locationOptionName_, parent->toolBarArea( this ) ); }

    }

    // change visibility
    setVisible( state );

}

//_______________________________________________________________
void CustomToolBar::_updateConfiguration( void )
{
    Debug::Throw( "CustomToolBar::_updateConfiguration.\n" );

    // pixmap size
    if( sizeFromOptions() )
    {
        int iconSize( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
        if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
        QToolBar::setIconSize( QSize( iconSize, iconSize ) );
    }

    // text label for toolbars
    const int toolButtonTextPosition( XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
    if( toolButtonTextPosition < 0 ) setToolButtonStyle(  (Qt::ToolButtonStyle) style()->styleHint( QStyle::SH_ToolButtonStyle ) );
    else setToolButtonStyle(  (Qt::ToolButtonStyle) toolButtonTextPosition );

    // lock
    if( lockFromOptions() ) {

        BaseMainWindow* mainwindow( qobject_cast<BaseMainWindow*>( window() ) );
        if( mainwindow && mainwindow->hasOptionName() && XmlOptions::get().contains( mainwindow->lockToolBarsOptionName() ) )
        { setMovable( !XmlOptions::get().get<bool>( mainwindow->lockToolBarsOptionName() ) ); }

    }

    // visibility
    bool currentVisibility( isVisible() );
    bool visibility( !optionName_.isEmpty() ? XmlOptions::get().get<bool>( optionName_ ):currentVisibility );

    // position
    // try cast parent to QMainWindow
    QMainWindow* parent( qobject_cast<QMainWindow*>( parentWidget() ) );
    if( parent && !optionName_.isEmpty() )
    {

        // get location from option
        Qt::ToolBarArea location = (XmlOptions::get().contains( locationOptionName_ )) ?
            (Qt::ToolBarArea)XmlOptions::get().get<int>( locationOptionName_ ) :
            Qt::TopToolBarArea;

        if( location == Qt::NoToolBarArea ) location = Qt::TopToolBarArea;

        Qt::ToolBarArea currentLocation = parent->toolBarArea( this );

        // some dump
        Debug::Throw() << "CustomToolBar::_updateConfiguration - " << optionName_ << " currentVisibility: " << currentVisibility << " currentLocation: " << areaToName( currentLocation ) << endl;
        Debug::Throw() << "CustomToolBar::_updateConfiguration - " << optionName_ << " visibility: " << visibility << " location: " << areaToName( location ) << endl;
        Debug::Throw() << endl;

        // set hidden if location is not specified
        if( location == Qt::NoToolBarArea ) visibility = false;

        // show toolbar
        if( visibility )
        {

            if( !( currentVisibility && (location == currentLocation) ) ) { parent->addToolBar( location, this ); }

        } else if( location != Qt::NoToolBarArea ) {

            parent->addToolBar( location, this );
            hide();

        }

        // set options according to values
        XmlOptions::get().set<bool>( optionName_, !isHidden() );
        XmlOptions::get().set<int>( locationOptionName_, parent->toolBarArea( this ) );

    }

    // update visibility action according to state for CustomToolbars
    Debug::Throw() << "CustomToolBar::_updateConfiguration -"
        << " optionName: " << optionName_
        << " visibility: " << visibility << endl;

    if( appearsInMenu() )
    {
        visibilityAction().setChecked( visibility );
        setVisible( visibility );
    }

}

//_______________________________________________________________
void CustomToolBar::_installActions( void )
{
    Debug::Throw( "CustomToolBar::_installActions.\n" );
    QString buffer;
    QTextStream( &buffer) << "&" << windowTitle();
    visibilityAction_ = new QAction( buffer, this );
    visibilityAction().setCheckable( true );

    // set default visibility option
    if( !( optionName_.isEmpty() || XmlOptions::get().contains( optionName_ ) ) )
    {
        XmlOptions::get().set<bool>( optionName_, true );
        visibilityAction().setChecked( true );
    }

    connect( visibilityAction_, SIGNAL( toggled( bool ) ), SLOT( _toggleVisibility( bool ) ) );
}

//_______________________________________________________________
CustomToolBar::AreaMap CustomToolBar::_initializeAreas( void )
{
    AreaMap out;
    out.insert( tr( "None" ), Qt::NoToolBarArea );
    out.insert( tr( "Left" ), Qt::LeftToolBarArea );
    out.insert( tr( "Right" ), Qt::RightToolBarArea );
    out.insert( tr( "Top" ), Qt::TopToolBarArea );
    out.insert( tr( "Bottom" ), Qt::BottomToolBarArea );
    return out;
}
