#ifndef TabWidget_p_h
#define TabWidget_p_h

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

#include <QLayout>
#include <QObject>

//* local dock widget
namespace Private
{
    class LocalTabWidget: public QWidget, private Base::Counter<LocalTabWidget>
    {
        Q_OBJECT

        public:

        //* constructor
        explicit LocalTabWidget( QWidget* );

        //* main widget
        QWidget* mainWidget( void )
        { return mainWidget_; }

        //* main layout
        QVBoxLayout* mainLayout( void )
        { return mainLayout_; }

        Q_SIGNALS:

        //* emitted when close event is received
        void closeEventRequest();

        protected:

        //* paint event
        virtual void paintEvent( QPaintEvent* );

        //* closeEvent
        virtual void closeEvent( QCloseEvent* event );

        //* resize event
        virtual void resizeEvent( QResizeEvent* );

        private:

        //* main widget
        QWidget* mainWidget_;

        //* layout
        QVBoxLayout* mainLayout_;

    };

}

#endif
