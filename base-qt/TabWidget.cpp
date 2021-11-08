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

#include "TabWidget.h"
#include "BaseContextMenu.h"
#include "File.h"
#include "QtUtil.h"
#include "TabWidget_p.h"
#include "XcbUtil.h"
#include "XmlOptions.h"


#include <QApplication>
#include <QGridLayout>
#include <QPainter>
#include <QSizeGrip>
#include <QStyleHintReturnMask>
#include <QStyleOption>
#include <QStyleOptionMenuItem>
#include <QStyleOptionFrame>

//________________________________________________________
TabWidget::TabWidget( QTabWidget* parent ):
    QWidget( parent ),
    Counter( QStringLiteral("TabWidget") ),
    parent_( parent ),
    widgetDragMonitor_( this )
{

    Debug::Throw( QStringLiteral("TabWidget::TabWidget.\n") );

    // dock
    dock_.reset( new Private::LocalTabWidget(nullptr) );
    dock_->setWindowIcon( windowIcon() );
    QtUtil::setMargin(dock_->mainLayout(), 0);
    dock_->mainLayout()->setSpacing(0);
    dock_->mainLayout()->addWidget( dockTitleLabel_ = new QLabel( dock_.get() ) );

    {
        QtUtil::setMargin(dockTitleLabel_, 5);
        dockTitleLabel_->setAlignment( Qt::AlignHCenter );
        dockTitleLabel_->setFont( QtUtil::titleFont( dockTitleLabel_->font() ) );
    }

    dock_->hide();

    _installActions();
    updateActions( false );

    // install dragMonitor on dock
    dock_->installEventFilter( &widgetDragMonitor_ );

    // connections
    connect( dock_.get(), &Private::LocalTabWidget::closeEventRequest, detachAction_, &QAction::trigger );
    connect( &widgetDragMonitor_, &WidgetDragMonitor::stateChangeRequest, this, &TabWidget::_toggleDock );

    // context menu
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, &QWidget::customContextMenuRequested, this, &TabWidget::_updateContextMenu );

}

//___________________________________________________________
TabWidget::~TabWidget() = default;

//___________________________________________________________
void TabWidget::setTitle( const QString &title )
{
    title_ = title;
    dock_->setWindowTitle( title );
    dockTitleLabel_->setText( title );
}

//___________________________________________________________
void TabWidget::updateActions( bool detached )
{

    detachAction_->setText( detached ? tr( "Attach" ): tr( "Detach" ) );
    stickyAction_->setEnabled( detached );
    staysOnTopAction_->setEnabled( detached );

}

//___________________________________________________________
void TabWidget::_toggleDock()
{

    Debug::Throw( QStringLiteral("TabWidget::_toggleDock.\n") );

    if( isDetached() )
    {

        widgetDragMonitor_.setEnabled( false );

        // update actions
        updateActions( false );

        // reinsert into parent and select
        parent_->QTabWidget::insertTab( index_, this, title_ );
        parent_->QTabWidget::setCurrentWidget( this );

        // hide dock
        dock_->hide();

        emit attached(true);
        emit attached();

    } else {

        // keep track of index, for later re-insertion
        // and remove from parent TabWidget
        index_ = parent_->indexOf( this );

        // move position
        const QPoint position( mapToGlobal( QPoint( 0, 0 ) ) );
        const QSize size( this->size() );

        // change parent
        setParent( dock_->mainWidget() );
        dock_->mainLayout()->addWidget( this );
        show();

        // adjust geometry
        dock_->setGeometry(
            QRect( position - QPoint( 0, dockTitleLabel_->height() ),
            QSize( size.width(), size.height() + dockTitleLabel_->height() ) ) );

        // show
        dock_->show();

        widgetDragMonitor_.setEnabled( true );

        // change action text
        updateActions( true );

        _toggleStaysOnTop( staysOnTopAction_->isChecked() );
        _toggleSticky( stickyAction_->isChecked() );

        // signal
        emit attached( false );
        emit detached();
    }

}

//__________________________________________________________
void TabWidget::_toggleStaysOnTop( bool state )
{

    // make sure that detached
    if( !isDetached() ) return;

    #if WITH_XCB

    // change property
    XcbUtil::get().changeState( this, XcbDefines::AtomId::_NET_WM_STATE_STAYS_ON_TOP, state );
    XcbUtil::get().changeState( this, XcbDefines::AtomId::_NET_WM_STATE_ABOVE, state );

    #else

    bool visible( !isHidden() );
    if( visible ) hide();

    // Qt implementation
    if( state ) setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
    else setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint );

    if( visible ) show();

    #endif

}

//__________________________________________________________
void TabWidget::_toggleSticky( bool state )
{

    Debug::Throw( QStringLiteral("TabWidget::_toggleSticky.\n") );

    // make sure that detached
    if( !isDetached() ) return;

    #if WITH_XCB
    if( XcbUtil::get().isSupported( XcbDefines::AtomId::_NET_WM_STATE_STICKY ) )
    {

        XcbUtil::get().changeState( this, XcbDefines::AtomId::_NET_WM_STATE_STICKY, state );

    } else if( XcbUtil::get().isSupported( XcbDefines::AtomId::_NET_WM_DESKTOP ) ) {

        unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), XcbDefines::AtomId::_NET_CURRENT_DESKTOP );
        XcbUtil::get().changeCardinal( this, XcbDefines::AtomId::_NET_WM_DESKTOP, state ? XcbDefines::ALL_DESKTOPS:desktop );

    }
    #endif
}

//______________________________________________________________________
void TabWidget::_updateContextMenu( QPoint position )
{

    Debug::Throw( QStringLiteral("TabWidget::_updateContextMenu.\n") );

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

//____________________________________________________
void TabWidget::_installActions()
{

    Debug::Throw( QStringLiteral("TabWidget::_installActions.\n") );

    // detach action
    addAction( detachAction_ = new QAction( tr( "Detach" ), this ) );
    detachAction_->setToolTip( tr( "Dock/undock panel" ) );
    connect( detachAction_, &QAction::triggered, this, &TabWidget::_toggleDock );

    // stays on top
    addAction( staysOnTopAction_ = new QAction( tr( "Keep Above" ), this ) );
    staysOnTopAction_->setToolTip( tr( "Keep window above all others" ) );
    staysOnTopAction_->setCheckable( true );
    staysOnTopAction_->setChecked( false );
    connect( staysOnTopAction_, &QAction::toggled, this, &TabWidget::_toggleStaysOnTop );

    // sticky
    addAction( stickyAction_ = new QAction( tr( "Sticky" ), this ) );
    stickyAction_->setToolTip( tr( "Make window appear on all desktops" ) );
    stickyAction_->setCheckable( true );
    stickyAction_->setChecked( false );
    connect( stickyAction_, &QAction::toggled, this, &TabWidget::_toggleSticky );

}

namespace Private
{

    //___________________________________________________________
    class SizeGrip: public QSizeGrip
    {

        Q_OBJECT

        public:

        explicit SizeGrip( QWidget* parent ):
            QSizeGrip( parent )
        {}

        protected:

        virtual void paintEvent( QPaintEvent* )
        {}

    };

    //___________________________________________________________
    LocalTabWidget::LocalTabWidget( QWidget* parent ):
        QWidget( parent, Qt::FramelessWindowHint|Qt::Window ),
        Counter( QStringLiteral("Private::LocalTabWidget") )
    {

        setProperty( "_KDE_NET_WM_FORCE_SHADOW", true );

        // grid layout to overlay main layout and invisible grip
        QGridLayout *gridLayout( new QGridLayout );
        QtUtil::setMargin(gridLayout, 0);
        gridLayout->setSpacing(0);
        setLayout( gridLayout );

        gridLayout->addWidget( mainWidget_ = new QWidget, 0, 0, 1, 1 );
        gridLayout->addWidget( new SizeGrip( this ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );

        mainWidget_->setLayout( mainLayout_ = new QVBoxLayout );

    }

    //___________________________________________________________
    void LocalTabWidget::paintEvent( QPaintEvent *event )
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
    void LocalTabWidget::resizeEvent( QResizeEvent* )
    {

        QStyleHintReturnMask menuMask;
        QStyleOption option;
        option.initFrom(this);
        if( style()->styleHint(QStyle::SH_Menu_Mask, &option, this, &menuMask) )
        { setMask(menuMask.region); }

    }

    //___________________________________________________________
    void LocalTabWidget::closeEvent( QCloseEvent* event )
    {
        Debug::Throw( 0, QStringLiteral("LocalWidget::closeEvent.\n") );
        event->ignore();
        emit closeEventRequest();
    }

}

#include "TabWidget.moc"
