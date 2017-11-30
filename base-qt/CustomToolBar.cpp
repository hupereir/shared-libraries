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

#include "BaseMainWindow.h"
#include "CppUtil.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "Singleton.h"
#include "XmlOptions.h"

//_______________________________________________________________
const CustomToolBar::AreaMap& CustomToolBar::_toolbarAreas()
{
    static const auto areas = Base::makeT<AreaMap>( {
        { tr( "None" ), Qt::NoToolBarArea },
        { tr( "Left" ), Qt::LeftToolBarArea },
        { tr( "Right" ), Qt::RightToolBarArea },
        { tr( "Top" ), Qt::TopToolBarArea },
        { tr( "Bottom" ), Qt::BottomToolBarArea }
    });
    return areas;
}

//_______________________________________________________________
CustomToolBar::CustomToolBar( const QString& title, QWidget* parent, const QString& optionName ):
    QToolBar( title, parent ),
    Counter( "CustomToolBar" ),
    optionName_( optionName ),
    locationOptionName_( optionName + "_LOCATION" ),
    appearsInMenu_( parent && qobject_cast<QMainWindow*>( parent ) )
{
    Debug::Throw( "CustomToolBar::CustomToolBar.\n" );

    // assign option name to object
    if( !optionName_.isEmpty() ) { setObjectName( optionName ); }

    _installActions();

    // configuration
    connect( Base::Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );
    _updateConfiguration();
}

//_______________________________________________________________
void CustomToolBar::setForcedVisible( bool value )
{
    forcedVisible_ = value;
    visibilityAction_->setEnabled( !value );
    visibilityAction_->setVisible( !value );
    if( value ) appearsInMenu_ = false;
}

//_______________________________________________________________
void CustomToolBar::paintEvent( QPaintEvent* event )
{ if( !transparent_ ) return QToolBar::paintEvent( event ); }

//_______________________________________________________________
void CustomToolBar::showEvent( QShowEvent* event )
{
    Debug::Throw() << "CustomToolBar::showEvent - name: " << optionName_ << endl;
    if( !isHidden() ) visibilityAction_->setChecked( true );
    QToolBar::showEvent(event);
}

//_______________________________________________________________
void CustomToolBar::hideEvent( QHideEvent* event )
{

    Debug::Throw() << "CustomToolBar::hideEvent - name: " << optionName_ << endl;
    if( isHidden() ) visibilityAction_->setChecked( false );
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

    // do nothing if forced visible
    if( forcedVisible_ ) return;

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
void CustomToolBar::_updateConfiguration()
{
    Debug::Throw( "CustomToolBar::_updateConfiguration.\n" );

    // pixmap size
    if( iconSizeFromOptions_ )
    {
        int iconSize( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
        if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
        QToolBar::setIconSize( QSize( iconSize, iconSize ) );
    }

    // text label for toolbars
    if( toolButtonStyleFromOptions_ )
    {
        const int toolButtonTextPosition( XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
        if( toolButtonTextPosition < 0 ) QToolBar::setToolButtonStyle(  (Qt::ToolButtonStyle) style()->styleHint( QStyle::SH_ToolButtonStyle ) );
        else QToolBar::setToolButtonStyle(  (Qt::ToolButtonStyle) toolButtonTextPosition );
    }

    // lock
    if( lockFromOptions_ )
    {
        BaseMainWindow* mainwindow( qobject_cast<BaseMainWindow*>( window() ) );
        if( mainwindow && mainwindow->hasOptionName() && XmlOptions::get().contains( mainwindow->lockToolBarsOptionName() ) )
        { QToolBar::setMovable( !XmlOptions::get().get<bool>( mainwindow->lockToolBarsOptionName() ) ); }
    }

    // visibility
    bool currentVisibility( isVisible() );
    bool visibility = !optionName_.isEmpty() ? XmlOptions::get().get<bool>( optionName_ ):currentVisibility;

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

        // set hidden if location is not specified
        if( location == Qt::NoToolBarArea ) visibility = false;

        // show toolbar
        if( forcedVisible_ )
        {
            if( location != Qt::NoToolBarArea ) parent->addToolBar( location, this );

        } else if( visibility ) {

            if( !( currentVisibility && (location == currentLocation) ) ) { parent->addToolBar( location, this ); }

        } else if( location != Qt::NoToolBarArea ) {

            parent->addToolBar( location, this );
            hide();

        }

        // set options according to values
        XmlOptions::get().set<bool>( optionName_, !isHidden() );
        XmlOptions::get().set<int>( locationOptionName_, parent->toolBarArea( this ) );

    }

    if( !forcedVisible_ )
    {
        visibilityAction_->setChecked( visibility );
        setVisible( visibility );
    }

}

//_______________________________________________________________
void CustomToolBar::_installActions()
{
    Debug::Throw( "CustomToolBar::_installActions.\n" );
    QString buffer;
    QTextStream( &buffer) << "&" << windowTitle();
    visibilityAction_ = new QAction( buffer, this );
    visibilityAction_->setCheckable( true );

    // set default visibility option
    if( !( optionName_.isEmpty() || XmlOptions::get().contains( optionName_ ) ) )
    {
        XmlOptions::get().set<bool>( optionName_, true );
        visibilityAction_->setChecked( true );
    }

    connect( visibilityAction_, SIGNAL(toggled(bool)), SLOT(_toggleVisibility(bool)) );
}
