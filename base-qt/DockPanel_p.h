#ifndef DockPanel_p_h
#define DockPanel_p_h

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
#include "WidgetDragMonitor.h"

#include <QBasicTimer>
#include <QEvent>
#include <QFrame>

//* local dock widget
namespace Private
{
    class LocalDockWidget: public QWidget, public Counter
    {
        Q_OBJECT

        public:

        //* constructor
        LocalDockWidget( QWidget* );

        protected:

        //* paint event
        virtual void paintEvent( QPaintEvent* );

    };


    //* local widget
    class LocalWidget: public QFrame, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        LocalWidget( QWidget* );

        //*@name accessors
        //@{

        //* true if attached
        bool isDetached( void ) const
        { return widgetDragMonitor_.isEnabled(); }

        //* widget drag monitor
        const WidgetDragMonitor& widgetDragMonitor( void ) const
        { return widgetDragMonitor_; }

        //@}

        //*@name modifiers
        //@{

        //* widget drag monitor
        WidgetDragMonitor& widgetDragMonitor( void )
        { return widgetDragMonitor_; }

        //* option name (needed to store sticky and stays-on-top state)
        void setOptionName( QString value )
        {
            stickyOptionName_ = value + "_STICKY";
            staysOnTopOptionName_ = value + "_STAYS_ON_TOP";
            _updateConfiguration();
        }

        //* detach
        void setDetached( bool value );

        //@}

        //*@name actions
        //@{

        //* detach action
        QAction& detachAction( void ) const
        { return *detachAction_; }

        //* stay on top
        QAction& staysOnTopAction( void ) const
        { return *staysOnTopAction_; }

        //* widget is hidden from taskbar
        QAction& stickyAction( void ) const
        { return *stickyAction_; }

        //@}

        protected:

        //* closeEvent
        virtual void closeEvent( QCloseEvent* event );

        //* resize event
        virtual void resizeEvent( QResizeEvent* );

        //* update actions
        void _updateActions( void );

        //* true if option name is set
        bool _hasOptionName( void ) const
        { return !(_stickyOptionName().isEmpty() || _staysOnTopOptionName().isEmpty() ); }

        //* option name
        const QString& _stickyOptionName( void ) const
        { return stickyOptionName_; }

        //* option name
        const QString& _staysOnTopOptionName( void ) const
        { return staysOnTopOptionName_; }

        protected Q_SLOTS:

        //* update context menu
        void _updateContextMenu( const QPoint& );

        //* toggle stays on top
        void _toggleStaysOnTop( bool );

        //* toggle sticky
        void _toggleSticky( bool );

        private Q_SLOTS:

        //* configuration
        void _updateConfiguration( void );

        private:

        //* actions
        void _installActions( void );

        //*@name actions
        //@{

        //* attach/detach action
        QAction* detachAction_ = nullptr;

        //* stay on top
        QAction* staysOnTopAction_ = nullptr;

        //* make window sticky
        QAction* stickyAction_ = nullptr;

        //@}

        //* option name
        /** needed to store sticky and stays on top state */
        QString stickyOptionName_;

        //* option name
        QString staysOnTopOptionName_;

        //* drag monitor
        WidgetDragMonitor widgetDragMonitor_;

    };

}

#endif