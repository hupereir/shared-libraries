
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

/*!
\file TabWidget.cpp
\brief Tab widget with detachable pages
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "TabWidget.h"

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QPainter>
#include <QtGui/QStyleHintReturnMask>
#include <QtGui/QStyleOption>
#include <QtGui/QStyleOptionMenuItem>
#include <QtGui/QStyleOptionFrame>

#include "File.h"
#include "XmlOptions.h"
#include "X11Util.h"

using namespace std;

//________________________________________________________
TabWidget::TabWidget( QTabWidget* parent ):
    QWidget(),
    Counter( "TabWidget" ),
    parent_( parent ),
    sizeGrip_( 0 ),
    index_( 0 ),
    button_( Qt::NoButton ),
    dragDistance_(6),
    dragDelay_( QApplication::doubleClickInterval() ),
    isDragging_( false )
{

    Debug::Throw( "TabWidget::TabWidget.\n" );

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground);
    setProperty( "_KDE_NET_WM_FORCE_SHADOW", true );

    // grid layout to overlay main layout and invisible grip
    QGridLayout *grid_layout( new QGridLayout() );
    grid_layout->setMargin(0);
    grid_layout->setSpacing(0);
    setLayout( grid_layout );

    grid_layout->addLayout( main_layout_ = new QVBoxLayout(), 0, 0, 1, 1 );
    main_layout_->setMargin(5);
    main_layout_->setSpacing(2);

    // vertical box
    box_ = new QWidget( this );
    box().setLayout( new QVBoxLayout() );
    box().layout()->setSpacing( 2 );
    box().layout()->setMargin( 0 );

    main_layout_->addWidget( box_ );

    {
        grid_layout->addWidget( sizeGrip_ = new SizeGrip( this ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );
        _hideSizeGrip();
    }

    _installActions();
    updateActions( false );
    setContextMenuPolicy( Qt::ActionsContextMenu );


}


//___________________________________________________________
void TabWidget::updateActions( bool detached )
{

    detachAction().setText( detached ? "&Attach":"&Detach" );
    stickyAction().setEnabled( detached );
    staysOnTopAction().setEnabled( detached );

}

//___________________________________________________________
void TabWidget::_toggleDock( void )
{

    Debug::Throw( "TabWidget::_toggleDock.\n" );

    if( !parent() ) {

        // store size for later detach
        updateActions( false );

        // reinsert into parent and select
        parent_->QTabWidget::insertTab( index_, this, title_ );
        parent_->QTabWidget::setCurrentWidget( this );

        // modify button text
        _hideSizeGrip();

        emit attached();

    } else {

        // keep track of index, for later re-insertion
        // and remove from parent TabWidget
        index_ = parent_->indexOf( this );

        // keep track of parent
        QWidget *parent( parentWidget() );

        // reparent to top level
        setParent( 0 );

        // window flags
        setWindowFlags( Qt::FramelessWindowHint|Qt::Window );

        // move and resize
        move( parent->mapToGlobal( QPoint(0,0) ) );

        // warning: innefficient
        setWindowIcon( QPixmap( XmlOptions::get().raw( "ICON_PIXMAP" ) ) );

        if( !title_.isEmpty() ) { setWindowTitle( title_ ); }

        // change action text
        updateActions( true );

        _toggleStaysOnTop( staysOnTopAction().isChecked() );
        _toggleSticky( stickyAction().isChecked() );

        // show widgets
        _showSizeGrip();
        show();

        // signal
        emit detached();
    }

}


//__________________________________________________________
void TabWidget::_toggleStaysOnTop( bool state )
{

    // check that widget is top level
    if( parentWidget() ) return;

    #if defined(Q_WS_X11)

    // change property
    X11Util::get().changeProperty( *this, X11Util::_NET_WM_STATE_STAYS_ON_TOP, state );
    X11Util::get().changeProperty( *this, X11Util::_NET_WM_STATE_ABOVE, state );

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

    Debug::Throw( "TabWidget::_toggleSticky.\n" );

    // check that widget is top level
    if( parentWidget() ) return;

    #if defined(Q_WS_X11)
    if( X11Util::get().isSupported( X11Util::_NET_WM_STATE_STICKY ) )
    {

        X11Util::get().changeProperty( *this, X11Util::_NET_WM_STATE_STICKY, state );

    } else if( X11Util::get().isSupported( X11Util::_NET_WM_DESKTOP ) ) {

        unsigned long desktop = X11Util::get().cardinal( QX11Info::appRootWindow(), X11Util::_NET_CURRENT_DESKTOP );
        X11Util::get().changeCardinal( *this, X11Util::_NET_WM_DESKTOP, state ? X11Util::ALL_DESKTOPS:desktop );

    }

    #endif

}

//___________________________________________________________
void TabWidget::closeEvent( QCloseEvent* event )
{
    Debug::Throw( "TabWidget::closeEvent.\n" );
    if( !parent() ) detachAction().trigger();
    event->ignore();
}

//___________________________________________________________
void TabWidget::mousePressEvent( QMouseEvent* event )
{
    Debug::Throw( "TabWidget::mousePressEvent.\n" );
    button_ = event->button();
    if( button_ == Qt::LeftButton )
    {
        event->accept();
        isDragging_ = false;
        dragPosition_ = event->pos();
        timer_.start( dragDelay_, this );
    }

    return;

}

//___________________________________________________________
void TabWidget::mouseReleaseEvent( QMouseEvent* event )
{
    Debug::Throw( "TabWidget::mouseReleaseEvent.\n" );
    event->accept();
    _resetDrag();
    return;

}

//___________________________________________________________
void TabWidget::mouseMoveEvent( QMouseEvent* event )
{

    // check button
    if( button_ != Qt::LeftButton ) return QWidget::mouseMoveEvent( event );

    timer_.stop();

    // check against drag distance
    if( QPoint( event->pos() - dragPosition_ ).manhattanLength() < dragDistance_ )
    { return QWidget::mouseMoveEvent( event ); }

    event->accept();
    if( !_startDrag() )
    { move( event->globalPos() - dragPosition_ ); }

}

//___________________________________________________________
void TabWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
    Debug::Throw( "TabWidget::mouseDoubleClickEvent.\n" );
    detachAction().trigger();
    timer_.stop();
}

//___________________________________________________________
void TabWidget::timerEvent( QTimerEvent *event )
{

    Debug::Throw( "TabWidget::timerEvent.\n" );
    if( event->timerId() != timer_.timerId() ) return QWidget::timerEvent( event );

    timer_.stop();
    if( button_ != Qt::LeftButton ) return;

    _startDrag();

}

//___________________________________________________________
void TabWidget::resizeEvent( QResizeEvent *event )
{

    QStyleHintReturnMask menuMask;
    QStyleOption option;
    option.initFrom(this);
    if( style()->styleHint(QStyle::SH_Menu_Mask, &option, this, &menuMask) )
    { setMask(menuMask.region); }

}

//___________________________________________________________
void TabWidget::paintEvent( QPaintEvent *event )
{
    if( parentWidget() ) { QWidget::paintEvent( event ); }
    else {

        QPainter p( this );

        if( !( style()->inherits( "OxygenStyle" ) || style()->inherits( "Oxygen::Style" ) ) )
        {  p.fillRect( event->rect(), palette().color( backgroundRole() ) ); }

        // background
        QStyleOptionMenuItem menuOpt;
        menuOpt.initFrom(this);
        menuOpt.state = QStyle::State_None;
        menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
        menuOpt.maxIconWidth = 0;
        menuOpt.tabWidth = 0;
        style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);

        // frame
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
        p.end();
    }

}

//____________________________________________________
void TabWidget::_installActions( void )
{

    Debug::Throw( "TabWidget::_installActions.\n" );

    // detach action
    addAction( detachAction_ = new QAction( "&Detach", this ) );
    detachAction_->setToolTip( "Dock/undock panel" );
    connect( detachAction_, SIGNAL( triggered() ), SLOT( _toggleDock() ) );

    // stays on top
    addAction( staysOnTopAction_ = new QAction( "&Stays on Top", this ) );
    staysOnTopAction_->setToolTip( "Keep window on top of all others" );
    staysOnTopAction_->setCheckable( true );
    staysOnTopAction_->setChecked( false );
    connect( staysOnTopAction_, SIGNAL( toggled( bool ) ), SLOT( _toggleStaysOnTop( bool ) ) );

    // sticky
    addAction( stickyAction_ = new QAction( "&Sticky", this ) );
    stickyAction_->setToolTip( "Make window appear on all desktops" );
    stickyAction_->setCheckable( true );
    stickyAction_->setChecked( false );
    connect( stickyAction_, SIGNAL( toggled( bool ) ), SLOT( _toggleSticky( bool ) ) );

}

//___________________________________________________________
bool TabWidget::_startDrag( void )
{

    if( parentWidget() ) {

        detachAction().trigger();
        qApp->processEvents();
        return _startDrag();

    } else if( !isDragging_ ) {

        isDragging_ = true;
        return X11Util::get().moveWidget( *this, mapToGlobal( dragPosition_ ) );

    } else return false;

}

//___________________________________________________________
void TabWidget::_resetDrag( void )
{
    unsetCursor();
    button_ = Qt::NoButton;
    dragPosition_ = QPoint();
    timer_.stop();
    isDragging_ = false;
}
