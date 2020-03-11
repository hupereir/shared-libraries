#ifndef breezewindowmanager_h
#define breezewindowmanager_h

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

#include "NonCopyable.h"

#include <QEvent>

#include <QApplication>
#include <QBasicTimer>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QWidget>

class WindowManager: public QObject, private Base::NonCopyable<WindowManager>
{

    Q_OBJECT

    public:

    //* singleton
    static WindowManager& get();

    //* register widget
    void registerWidget( QWidget* );

    //* event filter [reimplemented]
    bool eventFilter( QObject*, QEvent* ) override;

    private:

    //* constructor
    explicit WindowManager( QObject* = nullptr );

    //* initialize
    /** read relevant options from config */
    void initialize();

    //* timer event,
    /** used to start drag if button is pressed for a long enough time */
    void timerEvent( QTimerEvent* ) override;

    //* mouse press event
    bool mousePressEvent( QObject*, QEvent* );

    //* mouse move event
    bool mouseMoveEvent( QObject*, QEvent* );

    //* mouse release event
    bool mouseReleaseEvent( QObject*, QEvent* );

    //*@name configuration
    //@{

    //* enable state
    bool enabled() const
    { return enabled_; }

    //* enable state
    void setEnabled( bool value )
    { enabled_ = value; }

    //* returns true if window manager is used for moving
    bool useWMMoveResize() const
    { return supportWMMoveResize() && useWMMoveResize_; }

    //* use window manager for moving, when available
    void setUseWMMoveResize( bool value )
    { useWMMoveResize_ = value; }

    //* drag distance (pixels)
    void setDragDistance( int value )
    { dragDistance_ = value; }

    //* drag delay (msec)
    void setDragDelay( int value )
    { dragDelay_ = value; }

    //@}

    //* returns true if widget is dragable
    bool isDragable( QWidget* );

    //* returns true if drag can be started from current widget
    bool canDrag( QWidget* );

    //* returns true if drag can be started from current widget and position
    /** child at given position is passed as second argument */
    bool canDrag( QWidget*, QWidget*, const QPoint& );

    //* reset drag
    void resetDrag();

    //* start drag
    void startDrag( QWidget*, const QPoint& );

    //* X11 specific implementation for startDrag
    void startDragX11( QWidget*, const QPoint& );

    //* returns true if window manager is used for moving
    /** right now this is true only for X11 */
    bool supportWMMoveResize() const;

    //* utility function
    bool isDockWidgetTitle( const QWidget* ) const;

    //*@name lock
    //@{

    void setLocked( bool value )
    { locked_ = value; }

    //* lock
    bool isLocked() const
    { return locked_; }

    //@}

    //* returns first widget matching given class, or nullptr if none
    template<typename T> T findParent( const QWidget* ) const;

    //* enability
    bool enabled_ = true;

    //* use WM moveResize
    bool useWMMoveResize_ = true;

    //* drag distance
    /** this is copied from kwin::geometry */
    int dragDistance_ = QApplication::startDragDistance();

    //* drag delay
    /** this is copied from kwin::geometry */
    int dragDelay_ = QApplication::startDragTime();

    //* drag point
    QPoint dragPoint_;
    QPoint globalDragPoint_;

    //* drag timer
    QBasicTimer dragTimer_;

    //* target being dragged
    /** Weak pointer is used in case the target gets deleted while drag is in progress */
    QPointer<QWidget> target_;

    //* true if drag is about to start
    bool dragAboutToStart_ = false;

    //* true if drag is in progress
    bool dragInProgress_ = false;

    //* true if drag is locked
    bool locked_ = false;

    //* cursor override
    /** used to keep track of application cursor being overridden when dragging in non-WM mode */
    bool cursorOverride_ = false;

    //* application event filter
    QObject* appEventFilter_ = nullptr;

    //* used to handle add child events
    QObject* addChildEventFilter_ = nullptr;

    //* allow access of all private members to the app event filter
    friend class AddChildEventFilter;
    friend class AppEventFilter;

};

//____________________________________________________________________
template<typename T>
    T WindowManager::findParent( const QWidget* widget ) const
{

    if( !widget ) return nullptr;
    for( QWidget* parent = widget->parentWidget(); parent; parent = parent->parentWidget() )
    { if( T cast = qobject_cast<T>(parent) ) return cast; }

    return nullptr;
}

#endif
