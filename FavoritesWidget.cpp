// $Id$

/*******************************************************************************
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
*******************************************************************************/

#include "FavoritesWidget.h"
#include "FavoritesWidget_p.h"

#include "BaseFileIconProvider.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

const double FavoritesWidgetItem::BorderWidth = 2;

//___________________________________________________________________
void FavoritesWidgetItem::updateMinimumSize( void )
{

    // get icon size
    QSize size( 2*BorderWidth, 2*BorderWidth );

    // icon
    if( !icon().isNull() )
    {
        size.rwidth() += iconSize().width();
        size.rheight() = qMax<int>( size.height(), iconSize().height() + 2*BorderWidth );
    }

    // text
    if( !text().isEmpty() )
    {
        QSize textSize = fontMetrics().boundingRect( text() ).size();
        size.rwidth() += textSize.width();
        size.rheight() = qMax<int>( size.height(), textSize.height() + 2*BorderWidth );

        if( !icon().isNull() ) size.rwidth() += 2*BorderWidth;

    }

    // store
    setMinimumSize( size );

}

//___________________________________________________________________
bool FavoritesWidgetItem::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::HoverEnter: mouseOver_ = true; break;
        case QEvent::HoverLeave: mouseOver_ = false; break;
        default: break;
    }

    return QAbstractButton::event( event );

}

//___________________________________________________________________
void FavoritesWidgetItem::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    // render mouse over
    if( mouseOver_ )
    {

        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.showDecorationSelected = true;
        option.rect = rect();
        option.state |= QStyle::State_MouseOver;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, &painter, itemView_ );

    }

    // save layout direction
    const bool isRightToLeft( qApp->isRightToLeft() );

    // render text
    if( !text().isEmpty() )
    {

        QRect textRect( rect().adjusted( BorderWidth, BorderWidth, -BorderWidth, -BorderWidth ) );
        if( !icon().isNull() )
        {

            if( isRightToLeft ) textRect.setRight( textRect.right() - iconSize().width() - 2*BorderWidth );
            else textRect.setLeft( textRect.left() + iconSize().width() + 2*BorderWidth );

            // draw text
            painter.setFont( font() );
            painter.drawText( QRectF( textRect ), text(), QTextOption( Qt::AlignVCenter | (isRightToLeft ? Qt::AlignRight : Qt::AlignLeft ) ) );

        }

    }

    // render icon
    if( !icon().isNull() )
    {
        QRect iconRect( rect().adjusted( BorderWidth, BorderWidth, -BorderWidth, -BorderWidth ) );
        if( !text().isEmpty() )
        {
            if( isRightToLeft ) iconRect.setLeft( iconRect.right() - iconSize().width() );
            else iconRect.setRight( iconRect.left() + iconSize().width() );
        }

        // get pixmap
        const QPixmap pixmap( icon().pixmap( iconSize() ) );
        const QPoint position(
            iconRect.x() + 0.5*(iconRect.width() - pixmap.width()),
            iconRect.y() + 0.5*(iconRect.height() - pixmap.height()) );

        painter.drawPixmap( position, pixmap );

    }

    painter.end();

}

//___________________________________________________________________
FavoritesWidget::FavoritesWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "FavoritesWidget::FavoritesWidget" ),
    iconProvider_( 0x0 )
{

    Debug::Throw( "FavoritesWidget::FavoritesWidget.\n" );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(2);
    vLayout->setSpacing(0);
    setLayout( vLayout );

    // button layout
    buttonLayout_ = new QVBoxLayout();
    buttonLayout_->setMargin(0);
    buttonLayout_->setSpacing(0);

    vLayout->addLayout( buttonLayout_ );
    vLayout->addStretch( 1 );

    // button group
    group_ = new QButtonGroup( this );
    group_->setExclusive( false );
    connect( group_, SIGNAL( buttonClicked( QAbstractButton* ) ), SLOT( _buttonClicked( QAbstractButton* ) ) );

}

//______________________________________________________________________
void FavoritesWidget::add( const QString& name, const BaseFileInfo& fileInfo )
{
    add(
        iconProvider_ ? iconProvider_->icon( fileInfo ):QIcon(),
        name,
        fileInfo );
}

//___________________________________________________________________
void FavoritesWidget::add( const QIcon& icon, const QString& name, const BaseFileInfo& fileInfo )
{

    Debug::Throw( "FavoritesWidget::add.\n" );

    // create new item
    FavoritesWidgetItem* item = new FavoritesWidgetItem( this );
    item->setIcon( icon );
    item->setText( name );
    item->setFileInfo( fileInfo );

    // add to button group, list of items and layout
    group_->addButton( item );
    items_.append( item );
    buttonLayout_->addWidget( item );

}

//___________________________________________________________________
void FavoritesWidget::_buttonClicked( QAbstractButton* button )
{

    Debug::Throw( "FavoritesWidget::_buttonClicked.\n" );

    // try cast
    FavoritesWidgetItem* item = qobject_cast<FavoritesWidgetItem*>( button );
    if( !item ) return;

    emit itemSelected( item->fileInfo() );

}
