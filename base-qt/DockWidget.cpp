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

#include "DockWidget.h"
#include "DockWidget_p.h"
#include "BaseMainWindow.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QLayout>
#include <QStyle>

//_________________________________________________________
DockWidget::DockWidget(const QString& title, QWidget* parent, const QString& optionName ):
    QDockWidget(title, parent ),
    optionName_( optionName )
{

    Debug::Throw( "DockWidget::DockWidget.\n" );

    // assign option name to object
    if( !optionName_.isEmpty() )
    { setObjectName( optionName ); }

    _installActions();

    // setup container
    Private::MainWidget* main = new Private::MainWidget();
    main->windowMonitor().setMode( WidgetMonitor::Mode::Size );
    main->windowMonitor().setOptionName( optionName );
    main->setLayout( new QVBoxLayout );
    main->layout()->setMargin(0);
    main->layout()->setSpacing(0);
    setWidget( main );

    // no scroll area by default
    setUseScrollArea( false );

    // default features
    setFeatures( DockWidgetClosable|DockWidgetMovable );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &DockWidget::_updateConfiguration );
    _updateConfiguration();

}

//_________________________________________________________
void DockWidget::setUseScrollArea( bool value )
{

    // do nothing if unchanged
    if( container_ && value == useScrollArea_ ) return;

    Debug::Throw() << "DockWidget::setUseScrollArea - value: " << value << endl;

    useScrollArea_ = value;
    if( useScrollArea_ )
    {

        Private::ContainerScrollArea* container = new Private::ContainerScrollArea( widget() );

        if( mainWidget_ ) container->setWidget( mainWidget_ );

        container_.reset( container );
        widget()->layout()->addWidget( container_.get() );

    } else {

        Private::ContainerWidget* container = new Private::ContainerWidget( widget() );
        if( mainWidget_ )
        {
            mainWidget_->setParent( container );
            container->layout()->addWidget( mainWidget_ );
        }

        container_.reset( container );
        widget()->layout()->addWidget( container_.get() );

    }

}

//_________________________________________________________
void DockWidget::setFeatures( QDockWidget::DockWidgetFeatures features )
{
    Debug::Throw( "DockWidget::setFeatures.\n" );
    features_ = features;
    if( !locked_ ) QDockWidget::setFeatures( features );
}

//_________________________________________________________
void DockWidget::setLocked( bool locked )
{

    if( locked == locked_ ) return;

    Debug::Throw() << "DockWidget::setLocked - value: " << locked << endl;

    locked_ = locked;
    if( locked )
    {

        QDockWidget::setFeatures(QDockWidget::NoDockWidgetFeatures);

    } else {

        // restore saved features
        QDockWidget::setFeatures( features_ );

    }

    _updateTitleBarWidget();

}

//_______________________________________________________________
void DockWidget::setMainWidget( QWidget* mainWidget )
{

    Debug::Throw( "DockWidget::setMainWidget.\n" );

    // delete old mainWidget
    if( mainWidget_ )
    {
        mainWidget_->hide();
        mainWidget_->deleteLater();
    }

    // assign
    mainWidget_ = mainWidget;
    if( container_ )
    {

        if( useScrollArea_ ) qobject_cast<Private::ContainerScrollArea*>( container_.get() )->setWidget( mainWidget_ );
        else {

            mainWidget_->setParent( container_.get() );
            container_->layout()->addWidget( mainWidget_ );
        }

    }

}

//_________________________________________________________
void DockWidget::setAutoHideTitleBar( bool value )
{
    autoHideTitleBar_ = value;
    _updateTitleBarWidget();
}

//_________________________________________________________
void DockWidget::_updateTitleBarWidget()
{
    Debug::Throw( "DockWidget::_updateTitleBarWidget.\n" );

    if( autoHideTitleBar_ && locked_ )
    {

        if( !titleBarWidget_ ) titleBarWidget_.reset( new Private::TitleBarWidget() );
        setTitleBarWidget( titleBarWidget_.get() );

    } else {

        titleBarWidget_.reset();
        setTitleBarWidget( nullptr );

    }

}

//_______________________________________________________________
void DockWidget::_toggleVisibility( bool state )
{

    Debug::Throw() << "DockWidget::_toggleVisibility - name: " << optionName_ << " state: " << state << endl;
    if( !optionName_.isEmpty() ) XmlOptions::get().set<bool>( optionName_, state );

    if( parentWidget()->isVisible() || !state )
    { setVisible( state ); }

}

//_______________________________________________________________
void DockWidget::_updateConfiguration()
{
    Debug::Throw( "DockWidget::_updateConfiguration.\n" );

    // lock
    BaseMainWindow* mainwindow( qobject_cast<BaseMainWindow*>( window() ) );
    if( mainwindow && mainwindow->hasOptionName() && XmlOptions::get().contains( mainwindow->lockPanelsOptionName() ) )
    { setLocked( XmlOptions::get().get<bool>( mainwindow->lockPanelsOptionName() ) ); }

    if( !optionName_.isEmpty() && XmlOptions::get().contains( optionName_ ) )
    { visibilityAction_->setChecked( XmlOptions::get().get<bool>( optionName_ ) ); }

}

//_______________________________________________________________
void DockWidget::_installActions()
{
    Debug::Throw( "DockWidget::_installActions.\n" );

    visibilityAction_ = new QAction( windowTitle(), this );
    visibilityAction().setCheckable( true );

    // set default visibility option
    if( !( optionName_.isEmpty() || XmlOptions::get().contains( optionName_ ) ) )
    {
        XmlOptions::get().set<bool>( optionName_, true );
        visibilityAction().setChecked( true );
    }

    connect( visibilityAction_, &QAction::toggled, this, &DockWidget::_toggleVisibility );
}
