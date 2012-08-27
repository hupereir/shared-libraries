#ifndef _TabWidget_h_
#define _TabWidget_h_

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

#include "Counter.h"
#include "Debug.h"

#include <QtGui/QAction>
#include <QtCore/QBasicTimer>
#include <QtGui/QCloseEvent>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QSizeGrip>
#include <QtGui/QTabWidget>

#include <cassert>

// forward declaration
class TabWidget;

//! Tab Child container
class TabWidget: public QWidget, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    TabWidget( QTabWidget* parent );

    //! set tab title
    void setTitle( const QString& title )
    { title_ = title; }

    //! title
    const QString& title( void ) const
    { return title_; }

    //! get parent TabWidget
    QTabWidget& parentTabWidget( void )
    {
        assert( parent_ );
        return *parent_;
    }

    //! get box
    QWidget& box( void )
    {
        assert( box_ );
        return *box_;
    }

    //! update actions
    void updateActions( bool );

    //! detach action
    QAction& detachAction( void ) const
    { return *detachAction_; }

    //! stay on top
    QAction& staysOnTopAction( void ) const
    { return *staysOnTopAction_; }

    //! widget is hidden from taskbar
    QAction& stickyAction( void ) const
    { return *stickyAction_; }

    signals:

    //! emmited when box is detached
    void detached( void );

    //! emmited when box is attached
    void attached( void );

    protected:

    //! close event
    virtual void closeEvent( QCloseEvent* );

    //! mouse press event [overloaded]
    virtual void mousePressEvent( QMouseEvent* );

    //! mouse move event [overloaded]
    virtual void mouseMoveEvent( QMouseEvent* );

    //! mouse move event [overloaded]
    virtual void mouseReleaseEvent( QMouseEvent* );

    //! mouse move event [overloaded]
    virtual void mouseDoubleClickEvent( QMouseEvent* );

    //! timer event [overloaded]
    virtual void timerEvent( QTimerEvent* );

    //! resize
    virtual void resizeEvent( QResizeEvent* );

    //! paint
    virtual void paintEvent( QPaintEvent* );

    //! has size grip
    bool _hasSizeGrip( void ) const
    { return (bool)sizeGrip_; }

    //! size grip
    QSizeGrip& _sizeGrip( void ) const
    { return *sizeGrip_; }

    //! show size grip
    void _showSizeGrip( void )
    { if( _hasSizeGrip() ) _sizeGrip().show(); }

    //! hide size grip
    void _hideSizeGrip( void )
    { if( _hasSizeGrip() ) _sizeGrip().hide(); }

    //! start drag
    bool _startDrag( void );

    //! reset drag
    void _resetDrag( void );

    protected slots:

    //! toggle dock
    virtual void _toggleDock( void );

    //! stays on top
    virtual void _toggleStaysOnTop( bool );

    //! toggle window stickyness
    virtual void _toggleSticky( bool );

    private:

    //! actions
    void _installActions( void );

    //! flags
    unsigned int flags_;

    //! title
    QString title_;

    //! parent TabWidget
    QTabWidget* parent_;

    //! vertical layout
    QVBoxLayout* mainLayout_;

    //! contents vbox
    QWidget* box_;

    class SizeGrip: public QSizeGrip
    {
        public:

        SizeGrip( QWidget* parent ):
            QSizeGrip( parent )
        {}

        protected:

        virtual void paintEvent( QPaintEvent* )
        {}

    };

    //! size grip
    SizeGrip* sizeGrip_;

    //! index in parent tab
    int index_;

    //! attach/detach action
    QAction* detachAction_;

    //! stay on top
    QAction* staysOnTopAction_;

    //! make window sticky
    QAction* stickyAction_;

    //! button state
    Qt::MouseButton button_;

    //! move timer
    QBasicTimer timer_;

    //! click position
    QPoint dragPosition_;

    //! true when move is enabled
    bool isDragging_;

};
#endif
