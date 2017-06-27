#ifndef DockWidget_p_h
#define DockWidget_p_h

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

#include "WidgetMonitor.h"

#include <QLayout>
#include <QWidget>
#include <QScrollArea>
#include <QSize>

namespace Private
{
    //* container widget
    class MainWidget: public QWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit MainWidget( QWidget* parent = nullptr ):
            QWidget( parent ),
            monitor_( this )
        {}

        // monitor
        WidgetMonitor& windowMonitor()
        { return monitor_; }

        //* size hint
        QSize sizeHint() const override
        {
            const QSize size( monitor_.sizeHint() );
            return size.isValid() ? size:QWidget::sizeHint();
        }

        //* size hint
        QSize minimumSizeHint() const override
        { return sizeHint(); }

        private:

        //* window monitor
        WidgetMonitor monitor_;

    };

    //* title bar widget
    class TitleBarWidget: public QWidget
    {

        Q_OBJECT

        public:

        //* minimum size hint
        QSize minimumSizeHint() const override
        { return QSize( 1, 2 ); }

        //* minimum size hint
        QSize sizeHint() const override
        { return minimumSizeHint(); }

    };

    //* container widget
    class ContainerWidget: public QWidget
    {

        Q_OBJECT

        public:

        explicit ContainerWidget( QWidget* parent = nullptr ):
            QWidget( parent )
        {
            setLayout( new QVBoxLayout() );
            layout()->setSpacing(0);
            layout()->setMargin(0);
        }

    };

    //* container scroll area
    class ContainerScrollArea: public QScrollArea
    {

        Q_OBJECT

        public:

        explicit ContainerScrollArea( QWidget* parent ):
            QScrollArea( parent )
        {
            setWidgetResizable ( true );
            setFrameStyle( QFrame::NoFrame );
        }

    };

}

#endif
