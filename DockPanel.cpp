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

#include "BaseContextMenu.h"
#include "Debug.h"
#include "File.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include "XcbUtil.h"

#include "config-base-qt.h"

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
    dock_ = new Private::LocalDockWidget(0);
    dock_->setWindowIcon( windowIcon() );
    QVBoxLayout* vLayout( new QVBoxLayout() );
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addSpacing( 5 );
    vLayout->addWidget( dockTitleLabel_ = new QLabel( dock_ ) );
    dock_->setLayout( vLayout );

    {
        dockTitleLabel_->setAlignment( Qt::AlignHCenter );
        QFont font( dockTitleLabel_->font() );
        font.setWeight( QFont::Bold );
        dockTitleLabel_->setFont( font );
    }


    dock_->hide();

    // this layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin(0);
    layout()->setSpacing(0);

    // main widget
    layout()->addWidget( panel_ = new Private::LocalWidget( this ) );

    reinterpret_cast<Private::LocalWidget*>(panel_)->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
    connect( &reinterpret_cast<Private::LocalWidget*>(panel_)->detachAction(), SIGNAL(triggered()), SLOT(_toggleDock()) );

    // vertical layout for children
    mainLayout_ = new QVBoxLayout();
    mainLayout_->setMargin( 5 );
    mainLayout_->setSpacing( 5 );
    panel_->setLayout( mainLayout_ );

}

//___________________________________________________________
DockPanel::~DockPanel( void )
{

    Debug::Throw( "DockPanel::~DockPanel.\n" );
    dock_->deleteLater();

}

//___________________________________________________________
bool DockPanel::isDetached( void ) const
{ return reinterpret_cast<Private::LocalWidget*>(panel_)->isDetached(); }

//___________________________________________________________
void DockPanel::setOptionName( QString value )
{ return reinterpret_cast<Private::LocalWidget*>(panel_)->setOptionName( value ); }

//___________________________________________________________
void DockPanel::_toggleDock( void )
{

    Debug::Throw( "DockPanel::_toggleDock.\n" );

    if( reinterpret_cast<Private::LocalWidget*>(panel_)->isDetached() )
    {

        // change parent
        panel_->setParent( this );
        layout()->addWidget( panel_ );
        panel_->show();

        dock_->hide();
        reinterpret_cast<Private::LocalWidget*>(panel_)->setDetached( false );

        // signals
        emit attached( true );
        emit attached();

    } else {

        // change parent
        panel_->setParent( dock_ );
        dock_->layout()->addWidget( panel_ );
        panel_->show();

        dock_->show();

        // move and resize
        dock_->move( mapToGlobal( QPoint(0,0) ) );

        reinterpret_cast<Private::LocalWidget*>(panel_)->setDetached( true );

        // signals
        emit attached( false );
        emit detached();

    }

}

//___________________________________________________________
void DockPanel::hideEvent( QHideEvent* event )
{
    Debug::Throw( "DockPanel::hideEvent.\n" );
    emit visibilityChanged( false );
    QWidget::hideEvent( event );
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
    LocalWidget::LocalWidget( QWidget* parent ):
        QFrame( parent ),
        Counter( "Private::LocalWidget" ),
        clickCounter_( this, 2 ),
        button_( Qt::NoButton ),
        isDragging_( false ),
        isDetached_( false )
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

        detachAction_->setText( isDetached_ ? tr( "Attach" ):tr( "Detach" ) );
        stickyAction_->setEnabled( isDetached_ );
        staysOnTopAction_->setEnabled( isDetached_ );

    }

    //___________________________________________________________
    void LocalWidget::setDetached( bool value )
    {
        if( isDetached_ == value ) return;

        isDetached_ = value;
        _updateActions();

        if( isDetached_ )
        {

            // window flags
            setFrameStyle( QFrame::NoFrame );
            _toggleStaysOnTop( staysOnTopAction_->isChecked() );
            _toggleSticky( stickyAction_->isChecked() );

        } else setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    }

    //___________________________________________________________
    void LocalWidget::closeEvent( QCloseEvent* event )
    {
        Debug::Throw( "LocalWidget::closeEvent.\n" );
        if( isDetached_ )
        {
            detachAction_->trigger();
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
                detachAction_->trigger();
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
        { parentWidget()->move( event->globalPos() - dragPosition_ ); }

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
        if( !isDetached_ ) return;

        #if BASE_QT_HAVE_XCB

        XcbUtil::get().changeState( window(), X11Defines::_NET_WM_STATE_STAYS_ON_TOP, state );
        XcbUtil::get().changeState( window(), X11Defines::_NET_WM_STATE_ABOVE, state );

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
        if( !isDetached_ ) return;

        #if BASE_QT_HAVE_XCB
        if( XcbUtil::get().isSupported( X11Defines::_NET_WM_STATE_STICKY ) )
        {

            XcbUtil::get().changeState( window(), X11Defines::_NET_WM_STATE_STICKY, state );

        } else if( XcbUtil::get().isSupported( X11Defines::_NET_WM_DESKTOP ) ) {

            unsigned long desktop = XcbUtil::get().cardinal( XcbUtil::get().appRootWindow(), X11Defines::_NET_CURRENT_DESKTOP );
            XcbUtil::get().changeCardinal( window(), X11Defines::_NET_WM_DESKTOP, state ? X11Defines::ALL_DESKTOPS:desktop );

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

    //___________________________________________________________
    bool LocalWidget::_startDrag( void )
    {

        if( !isDetached_ )
        {

            detachAction_->trigger();
            return true;

        } else if( !isDragging_ ) {

            isDragging_ = true;

            #if BASE_QT_HAVE_XCB
            // && QT_VERSION < 0x050000
            if( XcbUtil::get().moveWidget( parentWidget(), mapToGlobal( dragPosition_ ) ) )
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

}
