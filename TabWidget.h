#ifndef TabWidget_h
#define TabWidget_h

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

#include "Counter.h"
#include "Debug.h"
#include "WidgetDragMonitor.h"

#include <QAction>
#include <QBasicTimer>
#include <QCloseEvent>
#include <QLabel>
#include <QLayout>
#include <QSizeGrip>
#include <QTabWidget>

// forward declaration
class TabWidget;

//* Tab Child container
class TabWidget: public QWidget, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    TabWidget( QTabWidget* parent );

    //* set tab title
    void setTitle( const QString& title )
    { title_ = title; }

    //* title
    const QString& title( void ) const
    { return title_; }

    //* get parent TabWidget
    QTabWidget& parentTabWidget( void )
    { return *parent_; }

    //* get box
    QWidget& box( void )
    { return *box_; }

    //* update actions
    void updateActions( bool );

    //* detach action
    QAction& detachAction( void ) const
    { return *detachAction_; }

    //* stay on top
    QAction& staysOnTopAction( void ) const
    { return *staysOnTopAction_; }

    //* widget is hidden from taskbar
    QAction& stickyAction( void ) const
    { return *stickyAction_; }

    Q_SIGNALS:

    //* emmited when box is detached
    void detached( void );

    //* emmited when box is attached
    void attached( void );

    protected:

    //* close event
    virtual void closeEvent( QCloseEvent* );

    //* resize
    virtual void resizeEvent( QResizeEvent* );

    //* paint
    virtual void paintEvent( QPaintEvent* );

    //* has size grip
    bool _hasSizeGrip( void ) const
    { return (bool)sizeGrip_; }

    //* size grip
    QSizeGrip& _sizeGrip( void ) const
    { return *sizeGrip_; }

    //* show size grip
    void _showSizeGrip( void )
    { if( _hasSizeGrip() ) _sizeGrip().show(); }

    //* hide size grip
    void _hideSizeGrip( void )
    { if( _hasSizeGrip() ) _sizeGrip().hide(); }

    protected Q_SLOTS:

    //* toggle dock
    virtual void _toggleDock( void );

    //* stays on top
    virtual void _toggleStaysOnTop( bool );

    //* toggle window stickyness
    virtual void _toggleSticky( bool );

    //* update context menu
    void _updateContextMenu( const QPoint& );

    private:

    //* actions
    void _installActions( void );

    //* flags
    unsigned int flags_;

    //* title
    QString title_;

    //* parent TabWidget
    QTabWidget* parent_;

    //* vertical layout
    QVBoxLayout* mainLayout_;

    //* contents vbox
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

    //* size grip
    SizeGrip* sizeGrip_;

    //* index in parent tab
    int index_;

    //*@name actions
    //@{

    //* attach/detach action
    QAction* detachAction_;

    //* stay on top
    QAction* staysOnTopAction_;

    //* make window sticky
    QAction* stickyAction_;

    //@}

    //* widget drag monitor
    WidgetDragMonitor widgetDragMonitor_;

};
#endif
