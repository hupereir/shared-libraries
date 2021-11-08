/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

#include "WindowManager.h"
#include "Debug.h"
#include "XcbUtil.h"


#include <QComboBox>
#include <QDialog>
#include <QDockWidget>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMouseEvent>
#include <QProgressBar>
#include <QScreen>
#include <QScrollBar>
#include <QStatusBar>
#include <QStyle>
#include <QStyleOptionGroupBox>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>

#include <QTextStream>

// needed to deal with device pixel ratio
#include <QWindow>

#if WITH_XCB
#include <QX11Info>
#include <xcb/xcb.h>
#endif

//* provide application-wise event filter
/**
it us used to unlock dragging and make sure event look is properly restored
after a drag has occurred
*/
class AppEventFilter: public QObject
{

    public:

    //* constructor
    explicit AppEventFilter( WindowManager* parent ):
        QObject( parent ),
        parent_( parent )
    {}

    //* event filter
    bool eventFilter( QObject* object, QEvent* event ) override
    {

        if( event->type() == QEvent::MouseButtonRelease )
        {

            // stop drag timer
            if( parent_->dragTimer_.isActive() )
            { parent_->resetDrag(); }

            // unlock
            if( parent_->isLocked() )
            { parent_->setLocked( false ); }

        }

        if( !parent_->enabled_ ) return false;

        /*
        if a drag is in progress, the widget will not receive any event
        we trigger on the first MouseMove or MousePress events that are received
        by any widget in the application to detect that the drag is finished
        */
        if( parent_->useWMMoveResize() && parent_->dragInProgress_ && parent_->target_ && ( event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress ) )
        { return appMouseEvent( object, event ); }

        return false;

    }

    private:

    //* application-wise event.
    /** needed to catch end of XMoveResize events */
    bool appMouseEvent( QObject*, QEvent* event )
    {

        Q_UNUSED( event );

        /*
        post some mouseRelease event to the target, in order to counter balance
        the mouse press that triggered the drag. Note that it triggers a resetDrag
        */
        QMouseEvent mouseEvent( QEvent::MouseButtonRelease, parent_->dragPoint_, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier );
        qApp->sendEvent( parent_->target_.data(), &mouseEvent );

        return false;

    }

    //* parent
    WindowManager* parent_ = nullptr;

};


class AddChildEventFilter: public QObject
{

    public:

    //* constructor
    explicit AddChildEventFilter( WindowManager* parent ):
        QObject( parent ),
        parent_( parent )
    {}

    //* event filter
    bool eventFilter( QObject*, QEvent* event ) override
    {

        if( event->type() == QEvent::ChildPolished )
        { parent_->registerWidget( qobject_cast<QWidget*>( static_cast<QChildEvent*>( event )->child() ) ); }

        return false;

    }

    private:

    //* parent
    WindowManager* parent_ = nullptr;

};

//________________________________________________________________________
WindowManager& WindowManager::get()
{
    static WindowManager singleton;
    return singleton;
}

//_____________________________________________________________
WindowManager::WindowManager( QObject* parent ):
    QObject( parent )
{

    // install application wise event filter
    appEventFilter_ = new AppEventFilter( this );
    qApp->installEventFilter( appEventFilter_ );

    addChildEventFilter_ = new AddChildEventFilter( this );

    initialize();

}

//_____________________________________________________________
void WindowManager::initialize()
{

    setEnabled( true );

    setDragDistance( QApplication::startDragDistance() );
    setDragDelay( QApplication::startDragTime() );

}

//_____________________________________________________________
void WindowManager::registerWidget( QWidget* widget )
{

    if( widget )
    {
        widget->removeEventFilter( addChildEventFilter_ );
        widget->installEventFilter( addChildEventFilter_ );
    }

    if( isDragable( widget ) )
    {

        /*
        install filter for dragable widgets.
        also install filter for blacklisted widgets
        to be able to catch the relevant events and prevent
        the drag to happen
        */
        widget->removeEventFilter( this );
        widget->installEventFilter( this );

    }

}

//_____________________________________________________________
bool WindowManager::eventFilter( QObject* object, QEvent* event )
{
    if( !enabled_ ) return false;

    switch ( event->type() )
    {
        case QEvent::MouseButtonPress:
        return mousePressEvent( object, event );
        break;

        case QEvent::MouseMove:
        if ( object == target_.data() ) return mouseMoveEvent( object, event );
        break;

        case QEvent::MouseButtonRelease:
        if ( target_ ) return mouseReleaseEvent( object, event );
        break;

        default:
        break;

    }

    return false;

}

//_____________________________________________________________
void WindowManager::timerEvent( QTimerEvent* event )
{

    if( event->timerId() == dragTimer_.timerId() )
    {

        dragTimer_.stop();
        if( target_ ) startDrag( target_.data()->window(), globalDragPoint_ );

    } else {

        return QObject::timerEvent( event );

    }

}

//_____________________________________________________________
bool WindowManager::mousePressEvent( QObject* object, QEvent* event )
{

    // cast event and check buttons/modifiers
    auto mouseEvent = static_cast<QMouseEvent*>( event );
    if (mouseEvent->source() != Qt::MouseEventNotSynthesized)
    { return false; }
    if( !( mouseEvent->modifiers() == Qt::NoModifier && mouseEvent->button() == Qt::LeftButton ) )
    { return false; }

    // check lock
    if( isLocked() ) return false;
    else setLocked( true );

    // cast to widget
    auto widget = static_cast<QWidget*>( object );

    // check if widget can be dragged from current position
    if( !canDrag( widget ) ) return false;

    // retrieve widget's child at event position
    auto position( mouseEvent->pos() );
    auto child = widget->childAt( position );
    if( !canDrag( widget, child, position ) ) return false;

    // save target and drag point
    target_ = widget;
    dragPoint_ = position;
    globalDragPoint_ = mouseEvent->globalPos();
    dragAboutToStart_ = true;

    // send a move event to the current child with same position
    // if received, it is caught to actually start the drag
    auto localPoint( dragPoint_ );
    if( child ) localPoint = child->mapFrom( widget, localPoint );
    else child = widget;
    QMouseEvent localMouseEvent( QEvent::MouseMove, localPoint, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier );
    qApp->sendEvent( child, &localMouseEvent );

    // never eat event
    return false;

}

//_____________________________________________________________
bool WindowManager::mouseMoveEvent( QObject* object, QEvent* event )
{

    Q_UNUSED( object );

    // stop timer
    if( dragTimer_.isActive() ) dragTimer_.stop();

    // cast event and check drag distance
    auto mouseEvent = static_cast<QMouseEvent*>( event );
    if (mouseEvent->source() != Qt::MouseEventNotSynthesized)
    { return false; }
    if( !dragInProgress_ )
    {

        if( dragAboutToStart_ )
        {
            if( mouseEvent->pos() == dragPoint_ )
            {
                // start timer,
                dragAboutToStart_ = false;
                if( dragTimer_.isActive() ) dragTimer_.stop();
                dragTimer_.start( dragDelay_, this );

            } else resetDrag();

        } else if( QPoint( mouseEvent->globalPos() - globalDragPoint_ ).manhattanLength() >= dragDistance_ ) {

            dragTimer_.start( 0, this );

        }

        return true;

    } else if( !useWMMoveResize() && target_ ) {

        // use QWidget::move for the grabbing
        /* this works only if the sending object and the target are identical */
        auto window( target_.data()->window() );
        window->move( window->pos() + mouseEvent->pos() - dragPoint_ );
        return true;

    } else return false;

}

//_____________________________________________________________
bool WindowManager::mouseReleaseEvent( QObject* object, QEvent* event )
{

    Q_UNUSED( object );
    Q_UNUSED( event );
    resetDrag();
    return false;
}

//_____________________________________________________________
bool WindowManager::isDragable( QWidget* widget )
{

    // check widget
    if( !widget ) return false;

    // accepted default types
    if(
        ( qobject_cast<QDialog*>( widget ) && widget->isWindow() ) ||
        ( qobject_cast<QMainWindow*>( widget ) && widget->isWindow() ) ||
        qobject_cast<QGroupBox*>( widget ) )
    { return true; }

    // more accepted types, provided they are not dock widget titles
    if( ( qobject_cast<QMenuBar*>( widget ) ||
        qobject_cast<QTabBar*>( widget ) ||
        qobject_cast<QStatusBar*>( widget ) ||
        qobject_cast<QToolBar*>( widget ) ) &&
        !isDockWidgetTitle( widget ) )
    { return true; }

    // flat toolbuttons
    if( auto toolButton = qobject_cast<QToolButton*>( widget ) )
    { if( toolButton->autoRaise() ) return true; }

    // viewports
    /*
    one needs to check that
    1/ the widget parent is a scrollarea
    2/ it matches its parent viewport
    3/ the parent is not blacklisted
    */
    if( auto listView = qobject_cast<QListView*>( widget->parentWidget() ) )
    { if( listView->viewport() == widget ) return true; }

    if( auto treeView = qobject_cast<QTreeView*>( widget->parentWidget() ) )
    { if( treeView->viewport() == widget ) return true; }

    /*
    catch labels in status bars.
    this is because of kstatusbar
    who captures buttonPress/release events
    */
    if( auto label = qobject_cast<QLabel*>( widget ) )
    {
        if( label->textInteractionFlags().testFlag( Qt::TextSelectableByMouse ) ) return false;

        QWidget* parent = label->parentWidget();
        while( parent )
        {
            if( qobject_cast<QStatusBar*>( parent ) ) return true;
            parent = parent->parentWidget();
        }
    }

    return false;

}


//_____________________________________________________________
bool WindowManager::canDrag( QWidget* widget )
{

    // check if enabled
    if( !enabled_ ) return false;

    // assume isDragable widget is already passed
    // check some special cases where drag should not be effective

    // check mouse grabber
    if( QWidget::mouseGrabber() ) return false;

    /*
    check cursor shape.
    Assume that a changed cursor means that some action is in progress
    and should prevent the drag
    */
    if( widget->cursor().shape() != Qt::ArrowCursor ) return false;

    // accept
    return true;

}

//_____________________________________________________________
bool WindowManager::canDrag( QWidget* widget, QWidget* child, const QPoint& position )
{

    // retrieve child at given position and check cursor again
    if( child && child->cursor().shape() != Qt::ArrowCursor ) return false;

    /*
    check against children from which drag should never be enabled,
    even if mousePress/Move has been passed to the parent
    */
    if( child && (
        qobject_cast<QComboBox*>(child ) ||
        qobject_cast<QProgressBar*>( child ) ||
        qobject_cast<QScrollBar*>( child ) ) )
    { return false; }

    // tool buttons
    if( auto toolButton = qobject_cast<QToolButton*>( widget ) )
    { return toolButton->autoRaise() && !toolButton->isEnabled(); }

    // check menubar
    if( auto menuBar = qobject_cast<QMenuBar*>( widget ) )
    {

        // do not drag from menubars embedded in Mdi windows
        if( findParent<QMdiSubWindow*>( widget ) ) return false;

        // check if there is an active action
        if( menuBar->activeAction() && menuBar->activeAction()->isEnabled() ) return false;

        // check if action at position exists and is enabled
        if( auto action = menuBar->actionAt( position ) )
        {
            if( action->isSeparator() ) return true;
            if( action->isEnabled() ) return false;
        }

        // return true in all other cases
        return true;

    }

    // tabbar. Make sure no tab is under the cursor
    if( auto tabBar = qobject_cast<QTabBar*>( widget ) )
    { return tabBar->tabAt( position ) == -1; }

    /*
    check groupboxes
    prevent drag if unchecking grouboxes
    */
    if( auto groupBox = qobject_cast<QGroupBox*>( widget ) )
    {
        // non checkable group boxes are always ok
        if( !groupBox->isCheckable() ) return true;

        // gather options to retrieve checkbox subcontrol rect
        QStyleOptionGroupBox opt;
        opt.initFrom( groupBox );
        if( groupBox->isFlat() ) opt.features |= QStyleOptionFrame::Flat;
        opt.lineWidth = 1;
        opt.midLineWidth = 0;
        opt.text = groupBox->title();
        opt.textAlignment = groupBox->alignment();
        opt.subControls = (QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxCheckBox);
        if (!groupBox->title().isEmpty()) opt.subControls |= QStyle::SC_GroupBoxLabel;

        opt.state |= (groupBox->isChecked() ? QStyle::State_On : QStyle::State_Off);

        // check against groupbox checkbox
        if( groupBox->style()->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxCheckBox, groupBox ).contains( position ) )
        { return false; }

        // check against groupbox label
        if( !groupBox->title().isEmpty() && groupBox->style()->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxLabel, groupBox ).contains( position ) )
        { return false; }

        return true;

    }

    // labels
    if( auto label = qobject_cast<QLabel*>( widget ) )
    { if( label->textInteractionFlags().testFlag( Qt::TextSelectableByMouse ) ) return false; }

    // abstract item views
    QAbstractItemView* itemView( nullptr );
    if(
        ( itemView = qobject_cast<QListView*>( widget->parentWidget() ) ) ||
        ( itemView = qobject_cast<QTreeView*>( widget->parentWidget() ) ) )
    {
        if( widget == itemView->viewport() )
        {
            // QListView
            if( itemView->frameShape() != QFrame::NoFrame ) return false;
            else if(
                itemView->selectionMode() != QAbstractItemView::NoSelection &&
                itemView->selectionMode() != QAbstractItemView::SingleSelection &&
                itemView->model() && itemView->model()->rowCount() ) return false;
            else if( itemView->model() && itemView->indexAt( position ).isValid() ) return false;
        }

    } else if( ( itemView = qobject_cast<QAbstractItemView*>( widget->parentWidget() ) ) ) {


        if( widget == itemView->viewport() )
        {
            // QAbstractItemView
            if( itemView->frameShape() != QFrame::NoFrame ) return false;
            else if( itemView->indexAt( position ).isValid() ) return false;
        }

    } else if( auto graphicsView =  qobject_cast<QGraphicsView*>( widget->parentWidget() ) )  {

        if( widget == graphicsView->viewport() )
        {
            // QGraphicsView
            if( graphicsView->frameShape() != QFrame::NoFrame ) return false;
            else if( graphicsView->itemAt( position ) ) return false;
        }

    }

    return true;

}

//____________________________________________________________
void WindowManager::resetDrag()
{

    if( (!useWMMoveResize() ) && target_ && cursorOverride_ ) {

        qApp->restoreOverrideCursor();
        cursorOverride_ = false;

    }

    target_.clear();
    if( dragTimer_.isActive() ) dragTimer_.stop();
    dragPoint_ = QPoint();
    globalDragPoint_ = QPoint();
    dragAboutToStart_ = false;
    dragInProgress_ = false;

}

//____________________________________________________________
void WindowManager::startDrag( QWidget* widget, const QPoint& position )
{

    if( !( enabled_ && widget ) ) return;
    if( QWidget::mouseGrabber() ) return;

    // ungrab pointer
    if( useWMMoveResize() )
    {
        #if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
        Q_UNUSED( position );
        widget->windowHandle()->startSystemMove();
        #else
        if( XcbUtil::isX11() ) startDragX11( widget, position );
        #endif
    } else if( !cursorOverride_ ) {

        qApp->setOverrideCursor( Qt::SizeAllCursor );
        cursorOverride_ = true;

    }
    
    dragInProgress_ = true;

}

//_______________________________________________________
void WindowManager::startDragX11( QWidget* widget, const QPoint& position )
{
    #if WITH_XCB
    XcbUtil::get().moveWidget( widget, position );
    #else
    Q_UNUSED( widget );
    Q_UNUSED( position );
    #endif
}

//____________________________________________________________
bool WindowManager::useWMMoveResize() const
{ 
    #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return true;
    #else
    return XcbUtil::isX11(); 
    #endif
}

//____________________________________________________________
bool WindowManager::isDockWidgetTitle( const QWidget* widget ) const
{

    if( !widget ) return false;
    if( auto dockWidget = qobject_cast<const QDockWidget*>( widget->parent() ) )
    {

        return widget == dockWidget->titleBarWidget();

    } else return false;

}
