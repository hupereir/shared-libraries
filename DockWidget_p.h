#ifndef DockWidget_p_h
#define DockWidget_p_h

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

#include "WidgetMonitor.h"

#include <QtGui/QLayout>
#include <QtGui/QWidget>
#include <QtGui/QScrollArea>
#include <QtCore/QSize>

//! container widget
class MainWidget: public QWidget
{
    public:

    //! constructor
    MainWidget( QWidget* parent = 0x0 ):
        QWidget( parent ),
        monitor_( this )
    {}

    // monitor
    WidgetMonitor& windowMonitor( void )
    { return monitor_; }

    //! size hint
    virtual QSize sizeHint( void ) const
    {
        const QSize size( monitor_.sizeHint() );
        return size.isValid() ? size:QWidget::sizeHint();
    }

    private:

    //! window monitor
    WidgetMonitor monitor_;

};

//! container widget
class ContainerWidget: public QWidget
{

    public:

    ContainerWidget( QWidget* parent = 0x0 ):
        QWidget( parent )
    {
        setLayout( new QVBoxLayout() );
        layout()->setSpacing(0);
        layout()->setMargin(0);
    }

};

//! container scroll area
class ContainerScrollArea: public QScrollArea
{

    public:

    ContainerScrollArea( QWidget* parent ):
        QScrollArea( parent )
    {
        setWidgetResizable ( true );
        setFrameStyle( QFrame::NoFrame );
    }

};

#endif
