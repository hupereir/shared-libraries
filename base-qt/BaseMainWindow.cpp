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

#include "BaseIconNames.h"
#include "DockWidget.h"
#include "IconEngine.h"
#include "IconSizeMenu.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "ToolBarMenu.h"
#include "ToolButtonStyleMenu.h"
#include "Util.h"

#include <QMenuBar>
#include <QTextStream>
#include <QToolBar>
#include <QWindowStateChangeEvent>

//____________________________________________________________
BaseMainWindow::BaseMainWindow( QWidget *parent, Qt::WindowFlags WindowFlags):
    QMainWindow( parent, WindowFlags ),
    monitor_( this )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::BaseMainWindow.\n") );

    // lock toolbars action
    addAction( lockToolBarsAction_ = new QAction( IconEngine::get( IconNames::Lock ), tr( "Lock Toolbar Positions" ), this ) );
    lockToolBarsAction_->setCheckable( true );
    lockToolBarsAction_->setChecked( true );
    connect( lockToolBarsAction_, &QAction::toggled, this, &BaseMainWindow::_lockToolBars );

    // lock panels action
    addAction( lockPanelsAction_ = new QAction( IconEngine::get( IconNames::Lock ), tr( "Lock Panel Positions" ), this ) );
    lockPanelsAction_->setCheckable( true );
    lockPanelsAction_->setChecked( true );
    connect( lockPanelsAction_, &QAction::toggled, this, &BaseMainWindow::_lockPanels );

    // show menu action
    addAction( showMenuBarAction_ = new QAction( IconEngine::get( IconNames::ShowMenu ), tr( "Show Menu Bar" ), this ) );
    showMenuBarAction_->setCheckable( true );
    showMenuBarAction_->setChecked( true );
    showMenuBarAction_->setShortcut( Qt::CTRL + Qt::Key_M );
    showMenuBarAction_->setEnabled( false );
    connect( showMenuBarAction_, &QAction::toggled, this, &BaseMainWindow::_toggleMenuBar );

    // show statusbar
    addAction( showStatusBarAction_ = new QAction( tr( "Show Status Bar" ), this ) );
    showStatusBarAction_->setCheckable( true );
    showStatusBarAction_->setChecked( true );
    showStatusBarAction_->setEnabled( false );
    connect( showStatusBarAction_, &QAction::toggled, this, &BaseMainWindow::_toggleStatusBar );

    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &BaseMainWindow::_updateConfiguration );
    connect( this, &BaseMainWindow::toolbarConfigurationChanged, &Base::Singleton::get(), &Base::Singleton::requestConfigurationChanged );

    _updateConfiguration();

}

//__________________________________________________
void BaseMainWindow::setOptionName( const QString& name )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::setOptionName.\n") );
    monitor_.setOptionName( name );

    if( name.isEmpty() ) {

        lockToolBarsOptionName_.clear();
        lockPanelsOptionName_.clear();
        showMenuBarOptionName_.clear();
        showStatusBarOptionName_.clear();

    } else {

        lockToolBarsOptionName_ = name+"_LOCK_TOOLBARS";
        if( !XmlOptions::get().contains( lockToolBarsOptionName_ ) ) XmlOptions::get().set<bool>( lockToolBarsOptionName_, lockToolBarsAction_->isChecked() );
        else { lockToolBarsAction_->setChecked( XmlOptions::get().get<bool>( lockToolBarsOptionName_ ) ); }

        lockPanelsOptionName_ = name+"_LOCK_PANELS";
        if( !XmlOptions::get().contains( lockPanelsOptionName_ ) ) XmlOptions::get().set<bool>( lockPanelsOptionName_, lockPanelsAction_->isChecked() );
        else { lockPanelsAction_->setChecked( XmlOptions::get().get<bool>( lockPanelsOptionName_ ) ); }

        showMenuBarOptionName_ = name+"_SHOW_MENU";
        if( !XmlOptions::get().contains( showMenuBarOptionName_ ) ) XmlOptions::get().set<bool>( showMenuBarOptionName_, showMenuBarAction_->isChecked() );
        else showMenuBarAction_->setChecked( XmlOptions::get().get<bool>( showMenuBarOptionName_ ) );

        showStatusBarOptionName_ = name+"_SHOW_STATUS";
        if( !XmlOptions::get().contains( showStatusBarOptionName_ ) ) XmlOptions::get().set<bool>( showStatusBarOptionName_, showStatusBarAction_->isChecked() );
        else showStatusBarAction_->setChecked( XmlOptions::get().get<bool>( showStatusBarOptionName_ ) );

    }

}

//__________________________________________________
void BaseMainWindow::setWindowTitle( const QString& title )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::setWindowTitle.\n") );
    QMainWindow::setWindowTitle( Util::windowTitle( title ) );

}

//__________________________________________________
void BaseMainWindow::setMenuBar( QMenuBar* menu )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::setMenuBar.\n") );
    QMainWindow::setMenuBar( menu );
    if( !menu ) return;
    menu->setVisible( showMenuBarAction_->isChecked() );
    showMenuBarAction_->setEnabled( true );
}

//__________________________________________________
void BaseMainWindow::setStatusBar( QStatusBar* widget )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::setStatusBar.\n") );
    QMainWindow::setStatusBar( widget );
    if( !statusBar() ) return;
    statusBar()->setVisible( showStatusBarAction_->isChecked() );
    showStatusBarAction_->setEnabled( true );
}

//__________________________________________________
QSize BaseMainWindow::minimumSizeHint() const
{
    const auto out( monitor_.sizeHint() );
    return out.isValid() ? out:QMainWindow::minimumSizeHint();
}

//__________________________________________________
QSize BaseMainWindow::sizeHint() const
{
    const auto out( monitor_.sizeHint() );
    return out.isValid() ? out:QMainWindow::sizeHint();
}

//________________________________________________________________
void BaseMainWindow::centerOnDesktop()
{

    Debug::Throw( QStringLiteral("BaseMainWindow::centerOnDesktop.\n") );
    move( QtUtil::centerOnDesktop( sizeHint() ) );

}

//________________________________________________________________
void BaseMainWindow::centerOnWidget( QWidget* parent )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::centerOnWidget.\n") );
    move( QtUtil::centerOnWidget( sizeHint(), parent ) );

}

//________________________________________________________________
QMenu* BaseMainWindow::createPopupMenu()
{
    Debug::Throw( QStringLiteral("BaseMainWindow::createPopupMenu.\n") );

    if( !_hasToolBars() )
    {

        _updateLayoutActions();

        auto menu = new QMenu( this );
        if( _hasPanels() ) menu->addAction( lockPanelsAction_ );
        if( _hasMenuBar() ) menu->addAction( showMenuBarAction_ );
        if( _hasStatusBar() ) menu->addAction( showStatusBarAction_ );
        return menu;

    } else {

        auto menu = toolBarMenu( this );
        menu->toolButtonStyleMenu().select( XmlOptions::get().get<int>( QStringLiteral("TOOLBUTTON_TEXT_POSITION") ) );
        menu->iconSizeMenu().select( (IconSize::Size) XmlOptions::get().get<int>( QStringLiteral("TOOLBUTTON_ICON_SIZE") ) );
        connect( &menu->toolButtonStyleMenu(), &ToolButtonStyleMenu::styleSelected, this, &BaseMainWindow::_updateToolButtonStyle );
        connect( &menu->iconSizeMenu(), &IconSizeMenu::iconSizeSelected, this, &BaseMainWindow::_updateToolButtonIconSize );
        return menu;

    }

}

//________________________________________________________________
ToolBarMenu* BaseMainWindow::toolBarMenu( QWidget* parent )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::toolBarMenu.\n") );

    _updateLayoutActions();

    auto menu = new ToolBarMenu( parent );
    const auto actions( _toolBarsActions( menu ) );
    if( actions.size() > 1 )
    {
        auto toolbarsMenu( menu->addMenu( tr( "Toolbars" ) ) );
        for( const auto& action:actions )
        { toolbarsMenu->addAction( action ); }
    }

    const bool hasToolButtonStyleToolBars( _hasToolButtonStyleToolBars() );
    const bool hasIconSizeToolBars( _hasIconSizeToolBars() );
    if( !hasToolButtonStyleToolBars ) menu->toolButtonStyleAction().setVisible( false );
    if( !hasIconSizeToolBars ) menu->iconSizeAction().setVisible( false );

    const bool hasLockableToolBars( _hasLockableToolBars() );
    const bool needSeparator( hasToolButtonStyleToolBars || hasIconSizeToolBars || hasLockableToolBars || _hasPanels() || _hasMenuBar() || _hasStatusBar() || actions.size() == 1 );
    if( needSeparator ) menu->addSeparator();
    if( hasLockableToolBars ) menu->addAction( lockToolBarsAction_ );

    if( _hasPanels() ) menu->addAction( lockPanelsAction_ );
    if( _hasMenuBar() ) menu->addAction( showMenuBarAction_ );
    if( actions.size() == 1 )
    {
        actions.front()->setText( tr( "Show Main Toolbar" ) );
        menu->addAction( actions.front() );
    }

    if( _hasStatusBar() ) menu->addAction( showStatusBarAction_ );

    return menu;

}

//________________________________________________________________
void BaseMainWindow::uniconify()
{

    Debug::Throw( QStringLiteral("BaseMainWindow::uniconify") );
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
            auto stateEvent( static_cast<QWindowStateChangeEvent*>(event) );

            if( windowState() & Qt::WindowMinimized )
            { _setWasMaximized( stateEvent->oldState() & Qt::WindowMaximized ); }

        }
        break;

        default: break;
    }

    return QMainWindow::event( event );

}

//________________________________________________________________
bool BaseMainWindow::_hasMenuBar() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasMenuBar.\n") );
    return (bool) findChild<QMenuBar*>();
}

//________________________________________________________________
bool BaseMainWindow::_hasStatusBar() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasStatusBar.\n") );
    return (bool) findChild<QStatusBar*>();
}

//________________________________________________________________
bool BaseMainWindow::_hasToolBars() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasToolBars.\n") );
    return !findChildren<QToolBar*>().isEmpty();
}

//________________________________________________________________
bool BaseMainWindow::_hasHideableToolBars() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasHideableToolBars.\n") );
    const auto toolbars( findChildren<QToolBar*>() );
    return std::any_of( toolbars.begin(), toolbars.end(), []( const QToolBar* toolbar )
    {
        if( toolbar->windowTitle().isEmpty() ) return false;
        else {
            auto customToolbar( qobject_cast<const CustomToolBar*>( toolbar ) );
            if( !(customToolbar && customToolbar->appearsInMenu() ) ) return false;
        }

        return true;
    } );
}

//________________________________________________________________
bool BaseMainWindow::_hasLockableToolBars() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasLockableToolBars.\n") );
    const auto toolbars( findChildren<QToolBar*>() );
    return std::any_of( toolbars.begin(), toolbars.end(), [this]( const QToolBar* toolbar )
    {
        if( toolbar->parentWidget() != this ) return false;
        else {

            auto customToolbar( qobject_cast<const CustomToolBar*>( toolbar ) );
            return !customToolbar || customToolbar->lockFromOptions();

        }

    } );

}

//________________________________________________________________
bool BaseMainWindow::_hasIconSizeToolBars() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasIconSizeToolBars.\n") );
    const auto toolbars( findChildren<QToolBar*>() );
    return std::any_of( toolbars.begin(), toolbars.end(), []( const QToolBar* toolbar )
    {
        auto customToolbar( qobject_cast<const CustomToolBar*>( toolbar ) );
        return !customToolbar || customToolbar->iconSizeFromOptions();
    } );

}


//________________________________________________________________
bool BaseMainWindow::_hasToolButtonStyleToolBars() const
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_hasToolButtonStyleToolBars.\n") );
    const auto toolbars( findChildren<QToolBar*>() );
    return std::any_of( toolbars.begin(), toolbars.end(), []( const QToolBar* toolbar )
    {

        auto customToolbar( qobject_cast<const CustomToolBar*>( toolbar ) );
        return !customToolbar || customToolbar->toolButtonStyleFromOptions();

    } );

}

//________________________________________________________________
bool BaseMainWindow::_hasPanels() const
{ return (bool) findChild<DockWidget*>(); }

//________________________________________________________________
BaseMainWindow::ActionList BaseMainWindow::_toolBarsActions( QMenu* menu )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_toolBarsActions.\n") );

    ActionList actions;

    auto toolbars( findChildren<QToolBar*>() );
    for( const auto& toolbar:toolbars )
    {

        // try cast to custom
        auto customToolbar( qobject_cast<CustomToolBar*>( toolbar ) );

        // skip toolbars that are not direct children
        // or should not appear in menu
        if( !(customToolbar && customToolbar->appearsInMenu() ))
        { continue; }

        if( customToolbar ) {

            actions.append( &customToolbar->visibilityAction() );

        } else if( !toolbar->windowTitle().isEmpty() ) {

            // add visibility action
            auto action = new QAction( toolbar->windowTitle(), menu );
            action->setCheckable( true );
            action->setChecked( toolbar->isVisible() );
            connect( action, &QAction::toggled, toolbar, &QWidget::setVisible );
            actions.append( action );

        }

    }

    return actions;

}

//________________________________________________________________
void BaseMainWindow::_updateStatusBarVisibility()
{

    Debug::Throw( QStringLiteral("BaseMainWindow::_updateStatusBarVisibility") );

    // loop over statusbar and adjust visibility
    for( auto statusBar:findChildren<QStatusBar*>() )
    { statusBar->setVisible( showStatusBarAction_->isChecked() ); }

}

//____________________________________________________________
void BaseMainWindow::_updateConfiguration()
{

    Debug::Throw( QStringLiteral("BaseMainWindow::_updateConfiguration.\n") );

    // icon size
    int iconSize( XmlOptions::get().get<int>( QStringLiteral("TOOLBUTTON_ICON_SIZE") ) );
    if( iconSize <= 0 ) iconSize = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    setIconSize( QSize( iconSize, iconSize ) );

    // text label for toolbars
    const int toolButtonTextPosition( XmlOptions::get().get<int>( QStringLiteral("TOOLBUTTON_TEXT_POSITION") ) );
    if( toolButtonTextPosition < 0 ) setToolButtonStyle(  (Qt::ToolButtonStyle) style()->styleHint( QStyle::SH_ToolButtonStyle ) );
    else setToolButtonStyle(  (Qt::ToolButtonStyle) toolButtonTextPosition );

    // toolbars locked
    if( hasOptionName() )
    {

        // toolbars locked
        if( XmlOptions::get().contains( lockToolBarsOptionName_ ) )
        { lockToolBarsAction_->setChecked( XmlOptions::get().get<bool>( lockToolBarsOptionName_ ) ); }

        // toolbars locked
        if( XmlOptions::get().contains( lockPanelsOptionName_ ) )
        { lockPanelsAction_->setChecked( XmlOptions::get().get<bool>( lockPanelsOptionName_ ) ); }

        // menu visibility
        if( XmlOptions::get().contains( showMenuBarOptionName_ ) )
        { showMenuBarAction_->setChecked( XmlOptions::get().get<bool>( showMenuBarOptionName_ ) ); }

        // menu visibility
        if( XmlOptions::get().contains( showStatusBarOptionName_ ) )
        { showStatusBarAction_->setChecked( XmlOptions::get().get<bool>( showStatusBarOptionName_ ) ); }

    }
}

//____________________________________________________________
void BaseMainWindow::_updateToolButtonStyle( int style )
{

    Debug::Throw( QStringLiteral("BaseMainWindow::_updateToolButtonStyle.\n") );
    XmlOptions::get().set<int>( QStringLiteral("TOOLBUTTON_TEXT_POSITION"), style );
    emit toolbarConfigurationChanged();

}

//____________________________________________________________
void BaseMainWindow::_updateToolButtonIconSize( IconSize::Size size )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_updateToolButtonIconSize.\n") );
    XmlOptions::get().set<int>( QStringLiteral("TOOLBUTTON_ICON_SIZE"), size );
    emit toolbarConfigurationChanged();
}

//____________________________________________________________
void BaseMainWindow::_updateLayoutActions()
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_updateLayoutActions.\n") );
    lockPanelsAction_->setEnabled( _hasPanels() );
    showMenuBarAction_->setEnabled( _hasMenuBar() );
    showStatusBarAction_->setEnabled( _hasStatusBar() );
}

//____________________________________________________________
void BaseMainWindow::_lockToolBars( bool value )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_lockToolBars.\n") );
    for( const auto& toolbar:findChildren<QToolBar*>() )
    {

        // skip if parent is not this
        if( toolbar->window() != this ) continue;

        // try cast to custom toolbar and check lock
        auto customToolBar( qobject_cast<CustomToolBar*>( toolbar ) );
        if( customToolBar && !customToolBar->lockFromOptions() ) continue;

        // update movability
        toolbar->setMovable( !value );

    }

    if( hasOptionName() ) XmlOptions::get().set<bool>( lockToolBarsOptionName_, value );
    return;
}

//____________________________________________________________
void BaseMainWindow::_lockPanels( bool value )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_lockPanels.\n") );
    for( const auto& panel:findChildren<DockWidget*>() )
    { panel->setLocked( value ); }

    if( hasOptionName() ) XmlOptions::get().set<bool>( lockPanelsOptionName_, value );
    return;
}

//____________________________________________________________
void BaseMainWindow::_toggleMenuBar( bool value )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_toggleMenuBar.\n") );

    if( !menuWidget() ) return;
    menuWidget()->setVisible( value );

    // save option
    if( hasOptionName() ) XmlOptions::get().set<bool>( showMenuBarOptionName_, value );

}

//____________________________________________________________
void BaseMainWindow::_toggleStatusBar( bool value )
{
    Debug::Throw( QStringLiteral("BaseMainWindow::_toggleStatusBar.\n") );

    // loop over statusbar and adjust visibility
    for( auto statusBar:findChildren<QStatusBar*>() )
    { statusBar->setVisible( value ); }

    // save option
    if( hasOptionName() ) XmlOptions::get().set<bool>( showStatusBarOptionName_, value );

}
