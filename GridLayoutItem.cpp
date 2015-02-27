
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

#include "GridLayoutItem.h"

#include "ElidedLabel.h"
#include "GridLayout.h"

//____________________________________________________________________________
GridLayoutItem::GridLayoutItem( QWidget* parent, GridLayout* layout, ItemFlags flags ):
    QObject( parent ),
    Counter( "GridLayoutItem" ),
    flags_( flags ),
    visible_( true )
{
    layout->addWidget( key_ = new QLabel( parent ), layout->currentRow(), layout->currentColumn(), Qt::AlignRight|Qt::AlignTop );

    // create correct value label
    if( flags & Elide ) value_ = new ElidedLabel( parent );
    else value_ = new QLabel( parent );

    if( flags & Selectable )
    {
        const Qt::TextInteractionFlags defaultFlags( QLabel().textInteractionFlags() );
        value_->setTextInteractionFlags( Qt::TextSelectableByMouse | defaultFlags );
    }

    // add to layout
    layout->addWidget( value_, layout->currentRow(), layout->currentColumn(), Qt::AlignLeft|Qt::AlignTop );

}

//____________________________________________________________________________
void GridLayoutItem::setKey( const QString& value )
{ key_->setText( value ); }

//____________________________________________________________________________
void GridLayoutItem::setText( const QString& value )
{

    // assign text
    ElidedLabel* label( qobject_cast<ElidedLabel*>( value_ ) );
    if( label )
    {

        label->ElidedLabel::setText( value );
        label->adjustSize();

    } else value_->setText( value );

    // update visibility
    setVisible( !value.isEmpty() );

}

//____________________________________________________________________________
void GridLayoutItem::setVisible( bool value )
{
    if( value ) show();
    else hide();
}

//____________________________________________________________________________
void GridLayoutItem::show( void )
{
    key_->show();
    value_->show();
    if( !visible_ )
    {
        visible_ = true;
        emit visibilityChanged( visible_ );
    }
}

//____________________________________________________________________________
void GridLayoutItem::hide( void )
{
    key_->hide();
    value_->hide();
    if( visible_ )
    {
        visible_ = false;
        emit visibilityChanged( visible_ );
    }
}
