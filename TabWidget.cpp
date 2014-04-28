
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

#include "TabWidget.h"
#include "TabWidget.moc"

#include "BaseContextMenu.h"
#include "File.h"
#include "XmlOptions.h"
#include "XcbUtil.h"

#include <QApplication>
#include <QGridLayout>
#include <QPainter>
#include <QStyleHintReturnMask>
#include <QStyleOption>
#include <QStyleOptionMenuItem>
#include <QStyleOptionFrame>

//________________________________________________________
TabWidget::TabWidget( QTabWidget* parent ):
    QWidget(),
    Counter( "TabWidget" ),
    parent_( parent ),
    sizeGrip_( 0 ),
    index_( 0 ),
    clickCounter_( this, 2 ),
    button_( Qt::NoButton ),
    isDragging_( false )
{

    Debug::Throw( "TabWidget::TabWidget.\n" );

    #if QT_VERSION < 0x050000
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground);
    #endif

    setProperty( "_KDE_NET_WM_FORCE_SHADOW", true );

    // grid layout to overlay main layout and invisible grip
    QGridLayout *gridLayout( new QGridLayout() );
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    setLayout( gridLayout );

    gridLayout->addLayout( mainLayout_ = new QVBoxLayout(), 0, 0, 1, 1 );
    mainLayout_->setMargin(5);
    mainLayout_->setSpacing(2);

    // vertical box
    box_ = new QWidget( this );
    box().setLayout( new QVBoxLayout() );
    box().layout()->setSpacing( 2 );
    box().layout()->setMargin( 0 );

    mainLayout_->addWidget( box_ );

    {
        gridLayout->addWidget( sizeGrip_ = new SizeGrip( this ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );
        _hideSizeGrip();
    }

    _installActions();
    updateActions( false );

    // context menu
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(_updateContextMenu(QPoint)) );

}


//___________________________________________________________
void TabWidget::updateActions( bool detached )
{

    detachAction().setText( detached ? tr( "Attach" ): tr( "Detach" ) );
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

    #if HAVE_X11

    // change property
    XcbUtil::get().changeState( this, X11Defines::_NET_WM_STATE_STAYS_ON_TOP, state );
    XcbUtil::get().changeState( this, X11Defines::_NET_WM_STATE_ABOVE, state );

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

    #if HAVE_X11
    if( XcbUtil::get().isSupported( X11Defines::_NET_WM_STATE_STICKY ) )
    {

        XcbUtil::get().changeState( this, X11Defines::_NET_WM_STATE_STICKY, state );

    } else if( XcbUtil::get().isSupported( X11Defines::_NET_WM_DESKTOP ) ) {

        unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), X11Defines::_NET_CURRENT_DESKTOP );
        XcbUtil::get().changeCardinal( this, X11Defines::_NET_WM_DESKTOP, state ? X11Defines::ALL_DESKTOPS:desktop );

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
    if( QPoint( event->pos() - dragPosition_ ).manhattanLength() < QApplication::startDragDistance() )
    { return QWidget::mouseMoveEvent( event ); }

    event->accept();
    if( !_startDrag() )
    { move( event->globalPos() - dragPosition_ ); }

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

//______________________________________________________________________
void TabWidget::_updateContextMenu( const QPoint& position )
{

    Debug::Throw( "TabWidget::_updateContextMenu.\n" );

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
void TabWidget::_installActions( void )
{

    Debug::Throw( "TabWidget::_installActions.\n" );

    // detach action
    addAction( detachAction_ = new QAction( tr( "Detach" ), this ) );
    detachAction_->setToolTip( tr( "Dock/undock panel" ) );
    connect( detachAction_, SIGNAL(triggered()), SLOT(_toggleDock()) );

    // stays on top
    addAction( staysOnTopAction_ = new QAction( tr( "Keep Above" ), this ) );
    staysOnTopAction_->setToolTip( tr( "Keep window above all others" ) );
    staysOnTopAction_->setCheckable( true );
    staysOnTopAction_->setChecked( false );
    connect( staysOnTopAction_, SIGNAL(toggled(bool)), SLOT(_toggleStaysOnTop(bool)) );

    // sticky
    addAction( stickyAction_ = new QAction( tr( "Sticky" ), this ) );
    stickyAction_->setToolTip( tr( "Make window appear on all desktops" ) );
    stickyAction_->setCheckable( true );
    stickyAction_->setChecked( false );
    connect( stickyAction_, SIGNAL(toggled(bool)), SLOT(_toggleSticky(bool)) );

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
void TabWidget::_resetDrag( void )
{
    unsetCursor();
    button_ = Qt::NoButton;
    dragPosition_ = QPoint();
    timer_.stop();
    isDragging_ = false;
}
