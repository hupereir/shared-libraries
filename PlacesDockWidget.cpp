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

#include "PlacesDockWidget.h"

#include "PlacesWidget.h"
#include "ScrollObject.h"
#include "Debug.h"

#include <QtGui/QScrollArea>

//_______________________________________________________________________
PlacesDockWidget::PlacesDockWidget( QWidget* parent ):
    DockWidget( "Places", parent )
{
    Debug::Throw( "PlacesDockWidget::PlacesDockWidget.\n" );

    // main widget
    QWidget* mainWidget = new QWidget( this );
    setWidget( mainWidget );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    mainWidget->setLayout( vLayout );

    // title
    title_ = new QLabel( "Places", mainWidget );
    title_->setMargin(3);
    vLayout->addWidget( title_ );

    // scroll area
    // create scroll area
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable ( true );
    scrollArea->setFrameStyle( QFrame::NoFrame );

    vLayout->addWidget( scrollArea );

    // add smooth scrolling object
    new ScrollObject( scrollArea );

    // create main widget
    // create main widget
    placesWidget_ = new PlacesWidget();
    scrollArea->setWidget( placesWidget_ );

}

//_______________________________________________________________________
void PlacesDockWidget::setLocked( bool value )
{
    DockWidget::setLocked( value );
    title_->setVisible( value );
}
