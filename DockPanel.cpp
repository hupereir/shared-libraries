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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "DockPanel.h"
#include "DockPanel.moc"
#include "DockPanel_p.h"

#include "Debug.h"
#include "File.h"
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

    // this layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin(0);
    layout()->setSpacing(2);

    // main widget
    layout()->addWidget( main_ = new LocalWidget( this ) );
    main_->setProperty( "_KDE_NET_WM_FORCE_SHADOW", true );

    reinterpret_cast<LocalWidget*>(main_)->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    connect( &reinterpret_cast<LocalWidget*>(main_)->detachAction(), SIGNAL(triggered()), SLOT(_toggleDock()) );

    // connections
    reinterpret_cast<LocalWidget*>(main_)->updateActions( false );
    connect( &reinterpret_cast<LocalWidget*>(main_)->stickyAction(), SIGNAL(toggled(bool)), SLOT(_toggleSticky(bool)) );
    connect( &reinterpret_cast<LocalWidget*>(main_)->staysOnTopAction(), SIGNAL(toggled(bool)), SLOT(_toggleStaysOnTop(bool)) );

    // vertical layout for children
    mainLayout_ = new QVBoxLayout();
    mainLayout_->setMargin( 5 );
    mainLayout_->setSpacing( 2 );
    main_->setLayout( mainLayout_ );

    // vertical panel
    panel_ = new QWidget( main_ );
    panel_->setLayout( new QVBoxLayout() );
    panel_->layout()->setMargin(0);
    panel_->layout()->setSpacing(2);

    mainLayout_->addWidget( panel_, 1 );

    // connections
    connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );

}

//___________________________________________________________
DockPanel::~DockPanel( void )
{

    Debug::Throw( "DockPanel::~DockPanel.\n" );

    // in detached mode, the panel must be explicitely deleted,
    // because it does not have the right parent
    if( !main_->parent() ) { main_->deleteLater(); }

}

//___________________________________________________________
void DockPanel::_toggleDock( void )
{

    Debug::Throw( "DockPanel::_toggleDock.\n" );

    if( !main_->parent() )
    {

        // change parent
        main_->setParent( this );
        layout()->addWidget( main_ );
        main_->show();

        // change action text
        reinterpret_cast<LocalWidget*>(main_)->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
        reinterpret_cast<LocalWidget*>(main_)->updateActions( false );

        // signals
        emit attached( true );
        emit attached();

    } else {

        // change parent
        main_->setParent( 0 );

        // window flags
        reinterpret_cast<LocalWidget*>(main_)->setFrameStyle( QFrame::NoFrame );
        main_->setWindowFlags( Qt::FramelessWindowHint|Qt::Window );

        // move and resize
        main_->move( mapToGlobal( QPoint(0,0) ) );

        // pixmap and title
        if( !title_.isEmpty() ) main_->setWindowTitle( title_ );

        // pass icon from application
        main_->setWindowIcon( windowIcon() );

        // change action text
        reinterpret_cast<LocalWidget*>(main_)->updateActions( true );

        _toggleStaysOnTop( reinterpret_cast<LocalWidget*>(main_)->staysOnTopAction().isChecked() );
        _toggleSticky( reinterpret_cast<LocalWidget*>(main_)->stickyAction().isChecked() );

        main_->show();

        // signals
        emit attached( false );
        emit detached();

    }

}

//__________________________________________________________
void DockPanel::_toggleStaysOnTop( bool state )
{

    Debug::Throw( "DockPanel::_toggleStaysOnTop.\n" );

    // check that widget is top level
    if( main_->parentWidget() ) return;

    #if HAVE_X11

    XcbUtil::get().changeState( main_, X11Defines::_NET_WM_STATE_STAYS_ON_TOP, state );
    XcbUtil::get().changeState( main_, X11Defines::_NET_WM_STATE_ABOVE, state );

    #else

    bool visible( !main_->isHidden() );
    if( visible ) main_->hide();

    // Qt implementation
    if( state ) main_->setWindowFlags( main_->windowFlags() | Qt::WindowStaysOnTopHint );
    else main_->setWindowFlags( main_->windowFlags() & ~Qt::WindowStaysOnTopHint );

    if( visible ) main_->show();

    #endif

    // update option if any
    if( _hasOptionName() ) XmlOptions::get().set<bool>( _staysOnTopOptionName(), state );

}

//__________________________________________________________
void DockPanel::_toggleSticky( bool state )
{

    Debug::Throw( "DockPanel::_toggleSticky.\n" );

    // check that widget is top level
    if( main_->parentWidget() ) return;

    #if HAVE_X11
    if( XcbUtil::get().isSupported( X11Defines::_NET_WM_STATE_STICKY ) )
    {

        XcbUtil::get().changeState( main_, X11Defines::_NET_WM_STATE_STICKY, state );

    } else if( XcbUtil::get().isSupported( X11Defines::_NET_WM_DESKTOP ) ) {

        unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), X11Defines::_NET_CURRENT_DESKTOP );
        XcbUtil::get().changeCardinal( main_, X11Defines::_NET_WM_DESKTOP, state ? X11Defines::ALL_DESKTOPS:desktop );

    }

    // update option if any
    if( _hasOptionName() ) XmlOptions::get().set<bool>( _stickyOptionName(), state );
    #endif

    return;

}

//___________________________________________________________
void DockPanel::_updateConfiguration( void )
{

    Debug::Throw( "DockPanel::_updateConfiguration.\n" );

    // sticky and stay on top options
    if( _hasOptionName() )
    {
        if( XmlOptions::get().contains( _staysOnTopOptionName() ) ) reinterpret_cast<LocalWidget*>(main_)->staysOnTopAction().setChecked( XmlOptions::get().get<bool>( _staysOnTopOptionName() ) );
        if( XmlOptions::get().contains( _stickyOptionName() ) ) reinterpret_cast<LocalWidget*>(main_)->stickyAction().setChecked( XmlOptions::get().get<bool>( _stickyOptionName() ) );
    }

}

//___________________________________________________________
void DockPanel::hideEvent( QHideEvent* event )
{
    Debug::Throw( "DockPanel::hideEvent.\n" );
    emit visibilityChanged( false );
    QWidget::hideEvent( event );
}

//___________________________________________________________
LocalWidget::LocalWidget( QWidget* parent ):
    QFrame( parent ),
    Counter( "LocalWidget" ),
    clickCounter_( this, 2 ),
    button_( Qt::NoButton ),
    isDragging_( false )
{
    _installActions();
    setContextMenuPolicy( Qt::ActionsContextMenu );
    #if QT_VERSION < 0x050000
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground);
    #endif

}

//___________________________________________________________
void LocalWidget::updateActions( bool detached )
{

    detachAction().setText( detached ? tr( "Attach" ): tr( "Detach" ) );
    stickyAction().setEnabled( detached );
    staysOnTopAction().setEnabled( detached );

}

//___________________________________________________________
void LocalWidget::closeEvent( QCloseEvent* event )
{
    Debug::Throw( "LocalWidget::closeEvent.\n" );
    if( !parent() )
    {
        detachAction().trigger();
        event->ignore();
    }
}

//___________________________________________________________
void LocalWidget::mousePressEvent( QMouseEvent* event )
{
    Debug::Throw( "LocalWidget::mousePressEvent.\n" );
    button_ = event->button();

    if( button_ == Qt::LeftButton )
    {
        event->accept();
        isDragging_ = false;
        dragPosition_ = event->pos();
        timer_.start( QApplication::doubleClickInterval(), this );

        // handle multiple clicks
        clickCounter_.increment();
        if( clickCounter_.counts() == 2 )
        {
            detachAction().trigger();
            timer_.stop();
        }

    }
    return;
}

//___________________________________________________________
void LocalWidget::mouseReleaseEvent( QMouseEvent* event )
{
    Debug::Throw( "LocalWidget::mouseReleaseEvent.\n" );
    event->accept();
    _resetDrag();
    return;
}

//___________________________________________________________
void LocalWidget::mouseMoveEvent( QMouseEvent* event )
{

    // check button
    if( button_ != Qt::LeftButton )
    { return QFrame::mouseMoveEvent( event ); }

    timer_.stop();

    // check against drag distance
    if( QPoint( event->pos() - dragPosition_ ).manhattanLength() < QApplication::startDragDistance() )
    { return QFrame::mouseMoveEvent( event ); }

    event->accept();
    if( !_startDrag() )
    { move( event->globalPos() - dragPosition_ ); }

}

//___________________________________________________________
void LocalWidget::timerEvent( QTimerEvent *event )
{

    Debug::Throw( "LocalWidget::timerEvent.\n" );
    if( event->timerId() != timer_.timerId() ) return QFrame::timerEvent( event );

    timer_.stop();
    if( button_ != Qt::LeftButton ) return;

    _startDrag();

}

//___________________________________________________________
void LocalWidget::resizeEvent( QResizeEvent *event )
{

    QStyleHintReturnMask menuMask;
    QStyleOption option;
    option.initFrom(this);
    if( style()->styleHint(QStyle::SH_Menu_Mask, &option, this, &menuMask) )
    { setMask(menuMask.region); }

}

//___________________________________________________________
void LocalWidget::paintEvent( QPaintEvent *event )
{
    if( parentWidget() ) return QFrame::paintEvent( event );
    else {

        QPainter painter( this );
        painter.setClipRegion( event->region() );

        if( !style()->inherits( "Oxygen::Style" ) )
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

    // sticky
    addAction( stickyAction_ = new QAction( tr( "Sticky" ), this ) );
    stickyAction_->setToolTip( tr( "Make window appear on all desktops" ) );
    stickyAction_->setCheckable( true );

}

//___________________________________________________________
bool LocalWidget::_startDrag( void )
{

    if( parentWidget() )
    {

        detachAction().trigger();
        return true;

    } else if( !isDragging_ ) {

        isDragging_ = true;

        #if HAVE_X11 && QT_VERSION < 0x050000
        if( XcbUtil::get().moveWidget( this, mapToGlobal( dragPosition_ ) ) )
        {

            _resetDrag();
            return true;

        } else return false;
        #else
        /*
        moving via XEvents is broken in Qt5.0,
        because one does not recieve a mouseRelease event at the end,
        and the next mousePress is therefore ignored.
        Trying to send a dummy mouseReleaseEvent does not cure the issue
        */
        return false;
        #endif

    } else return false;

}

//___________________________________________________________
void LocalWidget::_resetDrag( void )
{
    unsetCursor();
    button_ = Qt::NoButton;
    dragPosition_ = QPoint();
    timer_.stop();
    isDragging_ = false;
}
