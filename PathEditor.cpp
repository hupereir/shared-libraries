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
*
*******************************************************************************/

#include "PathEditor.h"
#include "Debug.h"

#include <QtCore/QDir>
#include <QtGui/QLayout>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QTextOption>

#include <cassert>

//____________________________________________________________________________
PathEditorItem::PathEditorItem( QWidget* parent ):
    QAbstractButton( parent ),
    Counter( "PathEditorItem" ),
    isRoot_( false ),
    isLast_( false ),
    mouseOver_( false )
{
    Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
    setAttribute( Qt::WA_Hover );
}

//____________________________________________________________________________
void PathEditorItem::setIsLast( bool value )
{
    if( isLast_ == value ) return;
    isLast_ = value;
    adjustSize();
    update();
}

//____________________________________________________________________________
void PathEditorItem::setPath( const File& path )
{

    path_ = path;
    if( QDir( path ).isRoot() )
    {

        isRoot_ = true;
        setText( "Root" );

    } else {

        // get local name
        File localName( path.localName() );
        if( localName.endsWith( "/" ) ) localName = localName.left( localName.size()-1 );
        assert( !localName.isEmpty() );

        setText( localName );

    }

}

//____________________________________________________________________________
QSize PathEditorItem::minimumSizeHint( void ) const
{
    QFont adjustedFont(font());
    adjustedFont.setBold( isLast_ );

    // text size
    QSize size( QFontMetrics( adjustedFont ).boundingRect( text() ).size() );

    // margins
    size.rwidth() += 2*BorderWidth;
    size.rheight() += 4*BorderWidth;

    // arrow width
    const int arrowWidth( _arrowWidth() );
    if( arrowWidth > 0 ) size.rwidth() += arrowWidth + 2*BorderWidth;

    return size;

}

//____________________________________________________________________________
QSize PathEditorItem::sizeHint( void ) const
{ return minimumSizeHint() + QSize( 4*BorderWidth, 0 ); }

//____________________________________________________________________________
bool PathEditorItem::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::HoverEnter: mouseOver_ = true; break;
        case QEvent::HoverLeave: mouseOver_ = false; break;
        default: break;
    }

    return QAbstractButton::event( event );

}

//____________________________________________________________________________
void PathEditorItem::paintEvent( QPaintEvent* event )
{

    QPainter painter( this );
    painter.setClipRegion( event->region() );

    // render text
    QRect textRect( rect() );
    if( !isLast_ ) textRect.adjust( 0, 0, -_arrowWidth()-2*BorderWidth, 0 );

    QFont adjustedFont(font());
    adjustedFont.setBold( isLast_ );
    painter.setFont( adjustedFont );
    painter.drawText( QRectF( textRect ), text(), QTextOption( Qt::AlignCenter ) );

    // render arrow
    if( !isLast_ )
    {
        QStyleOption option;
        option.initFrom(this);
        option.rect = QRect( textRect.width(), 0, rect().width()-textRect.width()-BorderWidth, rect().height() );
        option.palette = palette();
        style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &option, &painter, this);
    }

    // render mouse over
    if( mouseOver_ )
    {

        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.rect = rect();
        option.state |= QStyle::State_MouseOver;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, &painter, this );

    }

    painter.end();

}

//____________________________________________________________________________
int PathEditorItem::_arrowWidth( void ) const
{ return isLast_ ? 0:qMax( 4, fontMetrics().boundingRect(text()).height()/2 + BorderWidth ); }

//____________________________________________________________________________
PathEditor::PathEditor( QWidget* parent ):
    QWidget( parent ),
    Counter( "PathEditor" )
{

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->setMargin(0);
    setLayout( hLayout );

    group_ = new QButtonGroup( this );
    group_->setExclusive( false );
    connect( group_, SIGNAL( buttonClicked( QAbstractButton* ) ), SLOT( _buttonClicked( QAbstractButton* ) ) );

}

//____________________________________________________________________________
void PathEditor::setPath( const File& path )
{

    setUpdatesEnabled( false );

    // first remove all existing PathEditorItems
    foreach( PathEditorItem* item, findChildren<PathEditorItem*>() )
    {
        group_->removeButton( item );
        item->hide();
        item->deleteLater();
    }

    // get layout
    QHBoxLayout* layout( static_cast<QHBoxLayout*>( this->layout() ) );

    // create 'last' item
    PathEditorItem* item = new PathEditorItem( this );
    item->setPath( path );
    item->setIsLast( true );
    group_->addButton( item );
    layout->insertWidget( 0, item );

    QDir dir( path );
    while( dir.cdUp() )
    {
        PathEditorItem* item = new PathEditorItem( this );
        item->setPath( dir.path() );
        group_->addButton( item );
        layout->insertWidget( 0, item );
    }

    layout->addStretch( 1 );

    setUpdatesEnabled( true );

}

//____________________________________________________________________________
void PathEditor::_buttonClicked( QAbstractButton* button )
{ emit pathChanged( static_cast<PathEditorItem*>( button )->path() ); }
