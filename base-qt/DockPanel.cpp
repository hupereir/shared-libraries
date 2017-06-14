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

#include "DockPanel.h"
#include "DockPanel_p.h"

#include "BaseContextMenu.h"
#include "Debug.h"
#include "File.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include "XcbUtil.h"

#include <QApplication>
#include <QPainter>
#include <QStyleHintReturnMask>
#include <QStyleOption>
#include <QStyleOptionMenuItem>
#include <QStyleOptionDockWidget>
#include <QStyleOptionFrame>

//___________________________________________________________
DockPanel::DockPanel( QWidget* parent ):
    QWidget( parent ),
    Counter( "DockPanel" )
{
    Debug::Throw( "DockPanel::DockPanel.\n" );

    // dock
    dock_.reset( new Private::LocalDockWidget(nullptr) );
    dock_->setWindowIcon( windowIcon() );
    QVBoxLayout* vLayout( new QVBoxLayout );
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget( dockTitleLabel_ = new QLabel( dock_.get() ) );
    dock_->setLayout( vLayout );

    {
        dockTitleLabel_->setMargin(5);
        dockTitleLabel_->setAlignment( Qt::AlignHCenter );
        dockTitleLabel_->setFont( QtUtil::titleFont( dockTitleLabel_->font() ) );
    }

    dock_->hide();

    // this layout
    setLayout( new QVBoxLayout );
    layout()->setMargin(0);
    layout()->setSpacing(0);

    // main widget
    layout()->addWidget( panel_ = new Private::LocalWidget( this ) );

    // install dragMonitor on dock
    dock_->installEventFilter( &panel_->widgetDragMonitor() );

    // and connect close event request
    connect( dock_.get(), SIGNAL(closeEventRequest()), &panel_->detachAction(), SLOT(trigger()) );

    panel_->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    connect( &panel_->detachAction(), SIGNAL(triggered()), SLOT(_toggleDock()) );
    connect( &panel_->widgetDragMonitor(), SIGNAL(stateChangeRequest()), SLOT(_toggleDock()) );

    // vertical layout for children
    mainLayout_ = new QVBoxLayout;
    mainLayout_->setMargin( 5 );
    mainLayout_->setSpacing( 5 );
    panel_->setLayout( mainLayout_ );

}

//___________________________________________________________
DockPanel::~DockPanel( void ) = default;

//___________________________________________________________
QWidget& DockPanel::panel( void )
{ return *panel_; }

//___________________________________________________________
bool DockPanel::isDetached( void ) const
{ return panel_->isDetached(); }

//___________________________________________________________
void DockPanel::setTitle( QString title )
{
    dock_->setWindowTitle( title );
    dockTitleLabel_->setText( title );
}

//___________________________________________________________
void DockPanel::setOptionName( QString value )
{ return panel_->setOptionName( value ); }

//___________________________________________________________
void DockPanel::_toggleDock( void )
{

    Debug::Throw( "DockPanel::_toggleDock.\n" );

    if( panel_->isDetached() )
    {

        panel_->setDetached( false );

        // re show
        show();

        // change parent
        panel_->setParent( this );
        layout()->addWidget( panel_ );
        panel_->show();

        dock_->hide();

        // signals
        emit attached( true );
        emit attached();

    } else {

        // change parent
        const QPoint position( panel_->mapToGlobal( QPoint( 0, 0 ) ) );
        panel_->setParent( dock_.get() );
        dock_->layout()->addWidget( panel_ );
        panel_->show();

        dock_->show();

        // move and resize
        dock_->move( position - panel_->geometry().topLeft() );

        panel_->setDetached( true );
        hide();

        // signals
        emit attached( false );
        emit detached();

    }

}

namespace Private
{

    //___________________________________________________________
    LocalDockWidget::LocalDockWidget( QWidget* parent ):
        QWidget( parent, Qt::FramelessWindowHint|Qt::Window ),
        Counter( "Private::LocalDockWidget" )
    {

        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_StyledBackground);
        setProperty( "_KDE_NET_WM_FORCE_SHADOW", true );

    }

    //___________________________________________________________
    void LocalDockWidget::paintEvent( QPaintEvent *event )
    {
        QPainter painter( this );
        painter.setClipRegion( event->region() );

        if( !( style()->inherits( "Oxygen::Style" ) || style()->inherits( "Breeze::Style" ) ) )
        {  painter.fillRect( event->rect(), palette().color( backgroundRole() ) ); }

        {
            // background
            QStyleOptionMenuItem option;
            option.initFrom(this);
            option.state = QStyle::State_None;
            option.checkType = QStyleOptionMenuItem::NotCheckable;
            option.maxIconWidth = 0;
            option.tabWidth = 0;
            style()->drawPrimitive(QStyle::PE_PanelMenu, &option, &painter, this);
        }

        {
            // frame
            QStyleOptionFrame option;
            option.rect = rect();
            option.palette = palette();
            option.state = QStyle::State_None;
            option.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
            option.midLineWidth = 0;
            style()->drawPrimitive(QStyle::PE_FrameMenu, &option, &painter, this);
        }

    }

    //___________________________________________________________
    void LocalDockWidget::resizeEvent( QResizeEvent *event )
    {

        QStyleHintReturnMask menuMask;
        QStyleOption option;
        option.initFrom(this);
        if( style()->styleHint(QStyle::SH_Menu_Mask, &option, this, &menuMask) )
        { setMask(menuMask.region); }

    }

    //___________________________________________________________
    void LocalDockWidget::closeEvent( QCloseEvent* event )
    {
        Debug::Throw( 0, "LocalWidget::closeEvent.\n" );
        event->ignore();
        emit closeEventRequest();
    }

    //___________________________________________________________
    LocalWidget::LocalWidget( QWidget* parent ):
        QFrame( parent ),
        Counter( "Private::LocalWidget" ),
        widgetDragMonitor_( this )
    {
        _installActions();
        _updateActions();

        // context menu
        setContextMenuPolicy( Qt::CustomContextMenu );
        connect( this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(_updateContextMenu(QPoint)) );

        // configuration
        connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );
        _updateConfiguration();
    }

    //___________________________________________________________
    void LocalWidget::_updateActions( void )
    {

        detachAction_->setText( widgetDragMonitor_.isEnabled() ? tr( "Attach" ):tr( "Detach" ) );
        stickyAction_->setEnabled( widgetDragMonitor_.isEnabled() );
        staysOnTopAction_->setEnabled( widgetDragMonitor_.isEnabled() );

    }

    //___________________________________________________________
    void LocalWidget::setDetached( bool value )
    {

        if( widgetDragMonitor_.isEnabled() == value ) return;
        widgetDragMonitor_.setEnabled( value );

        _updateActions();

        if( widgetDragMonitor_.isEnabled() )
        {

            // window flags
            setFrameStyle( QFrame::NoFrame );
            _toggleStaysOnTop( staysOnTopAction_->isChecked() );
            _toggleSticky( stickyAction_->isChecked() );

        } else setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    }

    //______________________________________________________________________
    void LocalWidget::_updateContextMenu( const QPoint& position )
    {

        Debug::Throw( "LocalWidget::_updateContextMenu.\n" );

        // create menu
        BaseContextMenu menu( this );
        menu.setHideDisabledActions( true );

        // add entry
        menu.addAction( detachAction_ );
        menu.addAction( staysOnTopAction_ );
        menu.addAction( stickyAction_ );

        // execute
        menu.exec( mapToGlobal( position ) );
    }

    //__________________________________________________________
    void LocalWidget::_toggleStaysOnTop( bool state )
    {

        Debug::Throw( "LocalWidget::_toggleStaysOnTop.\n" );

        // check that widget is top level
        if( !widgetDragMonitor_.isEnabled() ) return;

        #if HAVE_XCB

        XcbUtil::get().changeState( window(), XcbDefines::_NET_WM_STATE_STAYS_ON_TOP, state );
        XcbUtil::get().changeState( window(), XcbDefines::_NET_WM_STATE_ABOVE, state );

        #else

        bool visible( !window()->isHidden() );
        if( visible ) window()->hide();

        // Qt implementation
        if( state ) window()->setWindowFlags( window()->windowFlags() | Qt::WindowStaysOnTopHint );
        else window()->setWindowFlags( window()->windowFlags() & ~Qt::WindowStaysOnTopHint );

        if( visible ) window()->show();

        #endif

        // update option if any
        if( _hasOptionName() ) XmlOptions::get().set<bool>( _staysOnTopOptionName(), state );

    }

    //__________________________________________________________
    void LocalWidget::_toggleSticky( bool state )
    {

        Debug::Throw( "LocalWidget::_toggleSticky.\n" );

        // check that widget is top level
        if( !widgetDragMonitor_.isEnabled() ) return;

        #if HAVE_XCB
        if( XcbUtil::get().isSupported( XcbDefines::_NET_WM_STATE_STICKY ) )
        {

            XcbUtil::get().changeState( window(), XcbDefines::_NET_WM_STATE_STICKY, state );

        } else if( XcbUtil::get().isSupported( XcbDefines::_NET_WM_DESKTOP ) ) {

            unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), XcbDefines::_NET_CURRENT_DESKTOP );
            XcbUtil::get().changeCardinal( window(), XcbDefines::_NET_WM_DESKTOP, state ? XcbDefines::ALL_DESKTOPS:desktop );

        }

        // update option if any
        if( _hasOptionName() ) XmlOptions::get().set<bool>( _stickyOptionName(), state );
        #endif

        return;

    }

    //___________________________________________________________
    void LocalWidget::_updateConfiguration( void )
    {

        Debug::Throw( "DockPanel::_updateConfiguration.\n" );

        // sticky and stay on top options
        if( _hasOptionName() )
        {
            if( XmlOptions::get().contains( _staysOnTopOptionName() ) ) staysOnTopAction_->setChecked( XmlOptions::get().get<bool>( _staysOnTopOptionName() ) );
            if( XmlOptions::get().contains( _stickyOptionName() ) ) stickyAction_->setChecked( XmlOptions::get().get<bool>( _stickyOptionName() ) );
        }

    }

    //___________________________________________________________
    void LocalWidget::_installActions( void )
    {

        Debug::Throw( "LocalWidget::_installActions.\n" );

        // detach
        addAction( detachAction_ = new QAction( tr( "Detach" ), this ) );
        detachAction_->setToolTip( tr( "Dock/undock panel" ) );

        // stays on top
        addAction( staysOnTopAction_ = new QAction( tr( "Keep Above" ), this ) );
        staysOnTopAction_->setToolTip( tr( "Keep window on top of all others" ) );
        staysOnTopAction_->setCheckable( true );
        connect( staysOnTopAction_, SIGNAL(toggled(bool)), SLOT(_toggleStaysOnTop(bool)) );

        // sticky
        addAction( stickyAction_ = new QAction( tr( "Sticky" ), this ) );
        stickyAction_->setToolTip( tr( "Make window appear on all desktops" ) );
        stickyAction_->setCheckable( true );
        connect( stickyAction_, SIGNAL(toggled(bool)), SLOT(_toggleSticky(bool)) );

    }

}
