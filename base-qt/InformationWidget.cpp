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

#include "InformationWidget.h"
#include "Color.h"
#include <QPainter>

//___________________________________________________________
InformationWidget::InformationWidget( QWidget* parent, const QString& text ):
QWidget( parent ),
Counter( "InformationWidget" )
{

    // create vbox layout
    auto layout=new QVBoxLayout;
    layout->setSpacing(10);
    layout->setMargin(10);
    setLayout( layout );

    // create message
    layout->addWidget( label_ = new QLabel( this ) );
    label_->setTextInteractionFlags( Qt::TextSelectableByMouse );
    label_->setText( text );

    // button layout
    buttonLayout_ = new QHBoxLayout;
    buttonLayout_->setMargin(0);
    buttonLayout_->setSpacing( 5 );
    layout->addLayout( buttonLayout_ );
    buttonLayout_->addStretch( 1 );

    adjustSize();

}

//___________________________________________________________
void InformationWidget::paintEvent( QPaintEvent* event )
{

    const auto baseColor = palette().color( QPalette::Active, QPalette::Window );
    const auto outlineColor = palette().color( QPalette::Active, QPalette::Highlight );
    Base::Color backgroundColor( outlineColor );

    const qreal alpha = 0.2;
    backgroundColor.merge( baseColor, alpha );

    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.setRenderHints( QPainter::Antialiasing );

    const qreal penWidth = 2;
    painter.setPen( QPen( outlineColor, penWidth ) );
    painter.setBrush( backgroundColor.get() );

    const qreal margin = penWidth/2 + 1;
    const qreal radius = 4;
    const auto rect = QRectF( this->rect() ).adjusted( margin, margin, -margin, -margin );
    painter.drawRoundedRect( rect, radius, radius );

}
