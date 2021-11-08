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
* this program.  If not, see <http://www.gnu.org/lic1enses/>.
*
*******************************************************************************/

#include "DockPanel.h"
#include "BaseContextMenu.h"
#include "Debug.h"
#include "DockPanel_p.h"
#include "File.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "XcbUtil.h"
#include "XmlOptions.h"



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
    Counter( QStringLiteral("DockPanel") )
{
    Debug::Throw( QStringLiteral("DockPanel::DockPanel.\n") );

    // dock
    dock_.reset( new Private::LocalDockWidget(nullptr) );
    dock_->setWindowIcon( windowIcon() );
    QVBoxLayout* vLayout( new QVBoxLayout );
    QtUtil::setMargin(vLayout, 0);
    vLayout->setSpacing(0);
    vLayout->addWidget( dockTitleLabel_ = new QLabel( dock_.get() ) );
    dock_->setLayout( vLayout );

    {
        QtUtil::setMargin(dockTitleLabel_, 5);
        dockTitleLabel_->setAlignment( Qt::AlignHCenter );
        dockTitleLabel_->setFont( QtUtil::titleFont( dockTitleLabel_->font() ) );
    }

    dock_->hide();

    // this layout
    setLayout( new QVBoxLayout );
    QtUtil::setMargin(layout(), 0);
    layout()->setSpacing(0);

    // main widget
    layout()->addWidget( panel_ = new Private::LocalWidget( this ) );

    // install dragMonitor on dock
    dock_->installEventFilter( &panel_->widgetDragMonitor() );

    // and connect close event request
    connect( dock_.get(), &Private::LocalDockWidget::closeEventRequest, &panel_->detachAction(), &QAction::trigger );

    panel_->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    connect( &panel_->detachAction(), &QAction::triggered, this, &DockPanel::_toggleDock );
    connect( &panel_->widgetDragMonitor(), &WidgetDragMonitor::stateChangeRequest, this, &DockPanel::_toggleDock );

    // vertical layout for children
    mainLayout_ = new QVBoxLayout;
    QtUtil::setMargin(mainLayout_, 5);
    mainLayout_->setSpacing( 5 );
    panel_->setLayout( mainLayout_ );

}

//___________________________________________________________
DockPanel::~DockPanel() = default;

//___________________________________________________________
QWidget& DockPanel::panel()
{ return *panel_; }

//___________________________________________________________
bool DockPanel::isDetached() const
{ return panel_->isDetached(); }

//___________________________________________________________
void DockPanel::setTitle( const QString &title )
{
    dock_->setWindowTitle( title );
    dockTitleLabel_->setText( title );
}

//___________________________________________________________
void DockPanel::setOptionName( const QString &value )
{ panel_->setOptionName( value ); }

//___________________________________________________________
void DockPanel::_toggleDock()
{

    Debug::Throw( QStringLiteral("DockPanel::_toggleDock.\n") );

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
        Counter( QStringLiteral("Private::LocalDockWidget") )
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
    void LocalDockWidget::resizeEvent( QResizeEvent* )
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
        Debug::Throw( 0, QStringLiteral("LocalWidget::closeEvent.\n") );
        event->ignore();
        emit closeEventRequest();
    }

    //___________________________________________________________
    LocalWidget::LocalWidget( QWidget* parent ):
        QFrame( parent ),
        Counter( QStringLiteral("Private::LocalWidget") ),
        widgetDragMonitor_( this )
    {
        _installActions();
        _updateActions();

        // context menu
        setContextMenuPolicy( Qt::CustomContextMenu );
        connect( this, &QWidget::customContextMenuRequested, this, &LocalWidget::_updateContextMenu );

        // configuration
        connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &LocalWidget::_updateConfiguration );
        _updateConfiguration();
    }

    //___________________________________________________________
    void LocalWidget::_updateActions()
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
    void LocalWidget::_updateContextMenu( QPoint position )
    {

        Debug::Throw( QStringLiteral("LocalWidget::_updateContextMenu.\n") );

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

        Debug::Throw( QStringLiteral("LocalWidget::_toggleStaysOnTop.\n") );

        // check that widget is top level
        if( !widgetDragMonitor_.isEnabled() ) return;

        #if WITH_XCB

        XcbUtil::get().changeState( window(), XcbDefines::AtomId::_NET_WM_STATE_STAYS_ON_TOP, state );
        XcbUtil::get().changeState( window(), XcbDefines::AtomId::_NET_WM_STATE_ABOVE, state );

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

        Debug::Throw( QStringLiteral("LocalWidget::_toggleSticky.\n") );

        // check that widget is top level
        if( !widgetDragMonitor_.isEnabled() ) return;

        #if WITH_XCB
        if( XcbUtil::get().isSupported( XcbDefines::AtomId::_NET_WM_STATE_STICKY ) )
        {

            XcbUtil::get().changeState( window(), XcbDefines::AtomId::_NET_WM_STATE_STICKY, state );

        } else if( XcbUtil::get().isSupported( XcbDefines::AtomId::_NET_WM_DESKTOP ) ) {

            unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), XcbDefines::AtomId::_NET_CURRENT_DESKTOP );
            XcbUtil::get().changeCardinal( window(), XcbDefines::AtomId::_NET_WM_DESKTOP, state ? XcbDefines::ALL_DESKTOPS:desktop );

        }

        // update option if any
        if( _hasOptionName() ) XmlOptions::get().set<bool>( _stickyOptionName(), state );
        #endif

        return;

    }

    //___________________________________________________________
    void LocalWidget::_updateConfiguration()
    {

        Debug::Throw( QStringLiteral("DockPanel::_updateConfiguration.\n") );

        // sticky and stay on top options
        if( _hasOptionName() )
        {
            if( XmlOptions::get().contains( _staysOnTopOptionName() ) ) staysOnTopAction_->setChecked( XmlOptions::get().get<bool>( _staysOnTopOptionName() ) );
            if( XmlOptions::get().contains( _stickyOptionName() ) ) stickyAction_->setChecked( XmlOptions::get().get<bool>( _stickyOptionName() ) );
        }

    }

    //___________________________________________________________
    void LocalWidget::_installActions()
    {

        Debug::Throw( QStringLiteral("LocalWidget::_installActions.\n") );

        // detach
        addAction( detachAction_ = new QAction( tr( "Detach" ), this ) );
        detachAction_->setToolTip( tr( "Dock/undock panel" ) );

        // stays on top
        addAction( staysOnTopAction_ = new QAction( tr( "Keep Above" ), this ) );
        staysOnTopAction_->setToolTip( tr( "Keep window on top of all others" ) );
        staysOnTopAction_->setCheckable( true );
        connect( staysOnTopAction_, &QAction::toggled, this, &LocalWidget::_toggleStaysOnTop );

        // sticky
        addAction( stickyAction_ = new QAction( tr( "Sticky" ), this ) );
        stickyAction_->setToolTip( tr( "Make window appear on all desktops" ) );
        stickyAction_->setCheckable( true );
        connect( stickyAction_, &QAction::toggled, this, &LocalWidget::_toggleSticky );

    }

}
