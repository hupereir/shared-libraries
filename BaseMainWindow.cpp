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

#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSizeMenu.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "ToolBarMenu.h"
#include "ToolButtonStyleMenu.h"
#include "Util.h"

#include <QtGui/QMenuBar>
#include <QtCore/QTextStream>
#include <QtGui/QToolBar>
#include <QtGui/QWindowStateChangeEvent>

//____________________________________________________________
BaseMainWindow::BaseMainWindow( QWidget *parent, Qt::WFlags wflags):
    QMainWindow( parent, wflags ),
    monitor_( this ),
    wasMaximized_( false )
{
    Debug::Throw( "BaseMainWindow::BaseMainWindow.\n" );

    // lock toolbars action
    addAction( lockToolBarsAction_ = new QAction( IconEngine::get( ICONS::LOCK ), "Lock Toolbars", this ) );
    lockToolBarsAction().setCheckable( true );
    lockToolBarsAction().setChecked( true );
    connect( &lockToolBarsAction(), SIGNAL( toggled( bool ) ), SLOT( _lockToolBars( bool ) ) );

    // show menu action
    addAction( showMenuBarAction_ = new QAction( IconEngine::get( ICONS::SHOW_MENU ), "Show Menu Bar", this ) );
    showMenuBarAction().setCheckable( true );
    showMenuBarAction().setChecked( true );
    showMenuBarAction().setShortcut( Qt::CTRL + Qt::Key_M );
    showMenuBarAction().setEnabled( false );
    connect( &showMenuBarAction(), SIGNAL( toggled( bool ) ), SLOT( _toggleMenuBar( bool ) ) );

    // show statusbar
    addAction( showStatusBarAction_ = new QAction( "Show Status Bar", this ) );
    showStatusBarAction().setCheckable( true );
    showStatusBarAction().setChecked( true );
    showStatusBarAction().setEnabled( false );
    connect( &showStatusBarAction(), SIGNAL( toggled( bool ) ), SLOT( _toggleStatusBar( bool ) ) );

    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    connect( this, SIGNAL( toolbarConfigurationChanged() ), Singleton::get().application(), SIGNAL( configurationChanged() ) );

    _updateConfiguration();

}

//__________________________________________________
void BaseMainWindow::setOptionName( const QString& name )
{

    Debug::Throw( "BaseMainWindow::setOptionName.\n" );
    monitor_.setOptionName( name );

    if( name.isEmpty() ) {

        lockToolBarsOptionName_.clear();
        showMenuBarOptionName_.clear();
        showStatusBarOptionName_.clear();

    } else {

        lockToolBarsOptionName_ = name+"_LOCK_TOOLBARS";
        if( !XmlOptions::get().contains( lockToolBarsOptionName() ) ) XmlOptions::get().set<bool>( lockToolBarsOptionName(), lockToolBarsAction().isChecked() );
        else { lockToolBarsAction().setChecked( XmlOptions::get().get<bool>( lockToolBarsOptionName() ) ); }

        showMenuBarOptionName_ = name+"_SHOW_MENU";
        if( !XmlOptions::get().contains( showMenuBarOptionName() ) ) XmlOptions::get().set<bool>( showMenuBarOptionName(), showMenuBarAction().isChecked() );
        else showMenuBarAction().setChecked( XmlOptions::get().get<bool>( showMenuBarOptionName() ) );

        showStatusBarOptionName_ = name+"_SHOW_STATUS";
        if( !XmlOptions::get().contains( showStatusBarOptionName() ) ) XmlOptions::get().set<bool>( showStatusBarOptionName(), showStatusBarAction().isChecked() );
        else showStatusBarAction().setChecked( XmlOptions::get().get<bool>( showStatusBarOptionName() ) );

    }

}

//__________________________________________________
void BaseMainWindow::setWindowTitle( const QString& title )
{

    Debug::Throw( "BaseMainWindow::setWindowTitle.\n" );
    QMainWindow::setWindowTitle( Util::windowTitle( title ) );

}

//__________________________________________________
void BaseMainWindow::setMenuBar( QMenuBar* menu )
{

    Debug::Throw( "BaseMainWindow::setMenuBar.\n" );
    QMainWindow::setMenuBar( menu );
    if( !menuBar() ) return;
    menuBar()->setVisible( showMenuBarAction().isChecked() );
    showMenuBarAction().setEnabled( true );
}

//__________________________________________________
void BaseMainWindow::setStatusBar( QStatusBar* widget )
{

    Debug::Throw( "BaseMainWindow::setStatusBar.\n" );
    QMainWindow::setStatusBar( widget );
    if( !statusBar() ) return;
    statusBar()->setVisible( showStatusBarAction().isChecked() );
    showStatusBarAction().setEnabled( true );
}

//__________________________________________________
QSize BaseMainWindow::minimumSizeHint( void ) const
{
    QSize out( monitor_.sizeHint() );
    return out.isValid() ? out:QMainWindow::minimumSizeHint();
}

//__________________________________________________
QSize BaseMainWindow::sizeHint( void ) const
{
    QSize out( monitor_.sizeHint() );
    return out.isValid() ? out:QMainWindow::sizeHint();
}

//________________________________________________________________
void BaseMainWindow::centerOnDesktop( void )
{

    Debug::Throw( "BaseMainWindow::centerOnDesktop.\n" );
    move( QtUtil::centerOnDesktop( sizeHint() ) );

}

//________________________________________________________________
void BaseMainWindow::centerOnWidget( QWidget* parent )
{

    Debug::Throw( "BaseMainWindow::centerOnWidget.\n" );
    move( QtUtil::centerOnWidget( sizeHint(), parent ) );

}

//________________________________________________________________
QMenu* BaseMainWindow::createPopupMenu( void )
{
    Debug::Throw( "BaseMainWindow::createPopupMenu.\n" );
    if( !_hasToolBars() )
    {

        QMenu* menu = new QMenu( this );
        if( _hasMenuBar() ) menu->addAction(&showMenuBarAction() );
        if( _hasStatusBar() ) menu->addAction(&showStatusBarAction() );
        return menu;

    } else {

        ToolBarMenu& menu = toolBarMenu( this );
        menu.toolButtonStyleMenu().select( XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
        menu.iconSizeMenu().select( (IconSize::Size) XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
        connect( &menu.toolButtonStyleMenu(), SIGNAL( styleSelected( int ) ), SLOT( _updateToolButtonStyle( int ) ) );
        connect( &menu.iconSizeMenu(), SIGNAL( iconSizeSelected( IconSize::Size ) ), SLOT( _updateToolButtonIconSize( IconSize::Size ) ) );
        return &menu;

    }

}

//________________________________________________________________
ToolBarMenu& BaseMainWindow::toolBarMenu( QWidget* parent )
{

    Debug::Throw( "BaseMainWindow::toolBarMenu.\n" );
    ToolBarMenu* menu = new ToolBarMenu( parent );

    const bool hasLockableToolbars( installToolBarsActions( *menu->addMenu( "ToolBars" ) ) );
    bool needSeparator( hasLockableToolbars || _hasMenuBar() || _hasStatusBar() );
    if( needSeparator ) menu->addSeparator();

    if( hasLockableToolbars ) menu->addAction( &lockToolBarsAction() );
    if( _hasMenuBar() ) menu->addAction( &showMenuBarAction() );
    if( _hasStatusBar() ) menu->addAction( &showStatusBarAction() );

    return *menu;

}

//________________________________________________________________
bool BaseMainWindow::installToolBarsActions( QMenu& menu )
{
    Debug::Throw( "BaseMainWindow::installToolBarsActions.\n" );

    bool hasLockableToolbars( false );
    QList<QToolBar*> toolbars( findChildren<QToolBar*>() );
    foreach( QToolBar* toolbar, toolbars )
    {

        // try cast to custom
        CustomToolBar* customToolbar( qobject_cast<CustomToolBar*>( toolbar ) );

        // skip toolbars that are not direct children
        // or should not appear in menu
        if( toolbar->parentWidget() != this && !(customToolbar && customToolbar->appearsInMenu() ))
        { continue; }

        if( customToolbar ) {

            Debug::Throw() << "BaseMainWindow::installToolBarsActions (custom) - " << toolbar->windowTitle() << endl;
            menu.addAction( &customToolbar->visibilityAction() );

        } else if( !toolbar->windowTitle().isEmpty() ) {

            // add visibility action
            Debug::Throw() << "BaseMainWindow::installToolBarsActions - " << toolbar->windowTitle() << endl;
            QAction* action = new QAction( toolbar->windowTitle(), &menu );
            action->setCheckable( true );
            action->setChecked( toolbar->isVisible() );
            connect( action, SIGNAL( toggled( bool ) ), toolbar, SLOT( setVisible( bool ) ) );
            menu.addAction( action );

        }

        // skip if lockable toolbar was already found
        if( hasLockableToolbars ) continue;

        // try cast to CustomToolBar and check for 'lock from options'
        if( customToolbar && customToolbar->lockFromOptions() ) continue;

        hasLockableToolbars = true;

    }

    return hasLockableToolbars;

}

//________________________________________________________________
void BaseMainWindow::uniconify( void )
{

    Debug::Throw( "BaseMainWindow::uniconify" );
    if( isMinimized() )
    {

        if( _wasMaximized() ) showMaximized();
        else showNormal();

    } else if( isHidden() ) show();

    activateWindow();
    raise();

}

//_______________________________________________________
bool BaseMainWindow::event( QEvent* event )
{

    // check that all needed widgets/actions are valid and checked.
    switch (event->type())
    {

        case QEvent::WindowStateChange:
        {
            // cast
            QWindowStateChangeEvent *state_event( static_cast<QWindowStateChangeEvent*>(event) );

            if( windowState() & Qt::WindowMinimized )
            { _setWasMaximized( state_event->oldState() & Qt::WindowMaximized ); }

        }
        break;

        default: break;
    }

    return QMainWindow::event( event );

}

//________________________________________________________________
bool BaseMainWindow::_hasMenuBar( void ) const
{
    Debug::Throw( "BaseMainWindow::_hasMenuBar.\n" );
    return (bool) qFindChild<QMenuBar*>( this );
}

//________________________________________________________________
bool BaseMainWindow::_hasStatusBar( void ) const
{
    Debug::Throw( "BaseMainWindow::_hasStatusBar.\n" );
    return (bool) qFindChild<QStatusBar*>( this );
}

//________________________________________________________________
bool BaseMainWindow::_hasToolBars( void ) const
{
    Debug::Throw( "BaseMainWindow::_hasToolBars.\n" );
    QList<QToolBar*> toolbars( qFindChildren<QToolBar*>( this ) );
    foreach( QToolBar* toolbar, toolbars )
    {

        // skip toolbars with no names
        if( toolbar->windowTitle().isEmpty() ) continue;

        // cast to custom
        CustomToolBar* customToolbar( qobject_cast<CustomToolBar*>( toolbar ) );

        // skip toolbars that are not direct children
        if( toolbar->parentWidget() != this && !(customToolbar && customToolbar->appearsInMenu() ))
        { continue; }

        return true;
    }

    return false;

}

//____________________________________________________________
void BaseMainWindow::_updateConfiguration( void )
{

    Debug::Throw( "BaseMainWindow::_updateConfiguration.\n" );

    // icon size
    int iconSize( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
    if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    setIconSize( QSize( iconSize, iconSize ) );

    // text label for toolbars
    const int toolButtonTextPosition( XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
    if( toolButtonTextPosition < 0 ) setToolButtonStyle(  (Qt::ToolButtonStyle) style()->styleHint( QStyle::SH_ToolButtonStyle ) );
    else setToolButtonStyle(  (Qt::ToolButtonStyle) toolButtonTextPosition );

    // toolbars locked
    if( hasOptionName() )
    {

        // toolbars locked
        if( XmlOptions::get().contains( lockToolBarsOptionName() ) )
        { lockToolBarsAction().setChecked( XmlOptions::get().get<bool>( lockToolBarsOptionName() ) ); }

        // menu visibility
        if( XmlOptions::get().contains( showMenuBarOptionName() ) )
        { showMenuBarAction().setChecked( XmlOptions::get().get<bool>( showMenuBarOptionName() ) ); }

        // menu visibility
        if( XmlOptions::get().contains( showStatusBarOptionName() ) )
        { showStatusBarAction().setChecked( XmlOptions::get().get<bool>( showStatusBarOptionName() ) ); }

    }
}

//____________________________________________________________
void BaseMainWindow::_updateToolButtonStyle( int style )
{

    Debug::Throw( "BaseMainWindow::_updateToolButtonStyle.\n" );
    XmlOptions::get().set<int>( "TOOLBUTTON_TEXT_POSITION", style );
    emit toolbarConfigurationChanged();

}

//____________________________________________________________
void BaseMainWindow::_updateToolButtonIconSize( IconSize::Size size )
{

    Debug::Throw( "BaseMainWindow::_updateToolButtonIconSize.\n" );
    XmlOptions::get().set<int>( "TOOLBUTTON_ICON_SIZE", size );
    emit toolbarConfigurationChanged();

}

//____________________________________________________________
void BaseMainWindow::_lockToolBars( bool value )
{
    Debug::Throw( "BaseMainWindow::_lockToolBars.\n" );
    foreach( QToolBar* toolbar, qFindChildren<QToolBar*>( this ) )
    {

        // skip if parent is not this
        if( !(toolbar->window() == this) ) continue;

        // try cast to CustomToolBar and check for 'lock from options'
        CustomToolBar* customtoolbar( qobject_cast<CustomToolBar*>( toolbar ) );
        if( customtoolbar && customtoolbar->lockFromOptions() ) continue;

        // update movability
        toolbar->setMovable( !value );

    }

    if( hasOptionName() ) XmlOptions::get().set<bool>( lockToolBarsOptionName(), value );
    return;
}

//____________________________________________________________
void BaseMainWindow::_toggleMenuBar( bool value )
{
    Debug::Throw( "BaseMainWindow::_toggleMenuBar.\n" );

    if( !menuWidget() ) return;
    menuWidget()->setVisible( value );

    // save option
    if( hasOptionName() ) XmlOptions::get().set<bool>( showMenuBarOptionName(), value );

}

//____________________________________________________________
void BaseMainWindow::_toggleStatusBar( bool value )
{
    Debug::Throw( "BaseMainWindow::_toggleStatusBar.\n" );

    if( !statusBar() ) return;
    statusBar()->setVisible( value );

    // save option
    if( hasOptionName() ) XmlOptions::get().set<bool>( showStatusBarOptionName(), value );

}
