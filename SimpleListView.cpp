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

#include "SimpleListView.h"
#include "SimpleListView.moc"
#include "SimpleListView_p.h"

#include "CustomPixmap.h"

#include <QAbstractItemDelegate>
#include <QApplication>
#include <QHeaderView>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOptionViewItem>
#include <QTextLayout>

//_________________________________________________________
SimpleListViewDelegate::SimpleListViewDelegate( QObject *parent ):
    QAbstractItemDelegate( parent ),
    Counter( "SimpleListViewDelegate" ),
    iconSize_( 32 )
{ Debug::Throw( "SimpleListViewDelegate::SimpleListViewDelegate.\n" ); }

//_________________________________________________________
void SimpleListViewDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    // check index
    if ( !index.isValid() ) return;

    const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
    const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const CustomPixmap pixmap( icon.pixmap( iconSize_, iconSize_ ) );
    const QPen oldPen = painter->pen();

    QFontMetrics fontMetrics = painter->fontMetrics();
    int pixmapWidth = pixmap.width()/pixmap.devicePixelRatio();
    int pixmapHeight = pixmap.height()/pixmap.devicePixelRatio();

    QTextLayout textLayout( text, option.font );
    QTextOption textOption( Qt::AlignHCenter );
    textLayout.setTextOption( textOption );
    const int maxWidth = qMax( 3 * pixmapWidth, 8 * fontMetrics.height() );
    _layoutText( &textLayout, maxWidth );

    QPalette::ColorGroup colorGroup = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;

    if( colorGroup == QPalette::Normal && !(option.state & QStyle::State_Active) )
    { colorGroup = QPalette::Inactive; }

    QStyleOptionViewItemV4 opt(option);
    opt.showDecorationSelected = true;
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    if ( option.state & QStyle::State_Selected )
    {
        painter->setPen( option.palette.color( colorGroup, QPalette::HighlightedText ) );

    } else {

        painter->setPen( option.palette.color( colorGroup, QPalette::Text ) );

    }

    if( !pixmap.isNull() )
    { painter->drawPixmap( option.rect.x() + (option.rect.width()/2)-(pixmapWidth/2), option.rect.y() + 5, pixmap ); }

    if ( !text.isEmpty() )
    { textLayout.draw( painter, QPoint( option.rect.x() + (option.rect.width()/2)-(maxWidth/2), option.rect.y() + pixmapHeight + 7 ) ); }

    // restore pen
    painter->setPen( oldPen );

    _drawFocus( painter, option, option.rect );
}

//_________________________________________________________
QSize SimpleListViewDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    if ( !index.isValid() ) return QSize();

    const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
    const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const CustomPixmap pixmap( icon.pixmap( iconSize_, iconSize_ ) );

    QFontMetrics fontMetrics = option.fontMetrics;
    int gap = fontMetrics.height();
    int pixmapHeight = 0;
    int pixmapWidth = iconSize_;

    if( !pixmap.isNull() )
    {
        pixmapHeight = pixmap.height()/pixmap.devicePixelRatio();
        pixmapWidth = pixmap.width()/pixmap.devicePixelRatio();
    }


    QTextLayout textLayout( text, option.font );
    const int maxWidth = qMax( 3 * pixmapWidth, 8 * fontMetrics.height() );
    const int textWidth = _layoutText( &textLayout, maxWidth );
    int textHeight = textLayout.boundingRect().height();

    int width, height;
    if ( text.isEmpty() ) height = pixmapHeight;
    else height = pixmapHeight + textHeight + 10;

    width = qMax( textWidth, pixmapWidth ) + 2*gap;

    return QSize( width, height );
}

//_________________________________________________________
int SimpleListViewDelegate::_layoutText(QTextLayout *layout, int maxWidth) const
{
    qreal height = 0;
    int textWidth = 0;
    layout->beginLayout();
    while( true )
    {
        QTextLine line = layout->createLine();
        if( !line.isValid() ) break;
        line.setLineWidth(maxWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        textWidth = qMax(textWidth, qRound(line.naturalTextWidth() + 0.5));
    }

    layout->endLayout();
    return textWidth;
}

//_________________________________________________________
void SimpleListViewDelegate::_drawFocus( QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const
{
    if (option.state & QStyle::State_HasFocus)
    {

        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(option);
        o.rect = rect;
        o.state |= QStyle::State_KeyboardFocusChange;
        QPalette::ColorGroup colorGroup = (option.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
        o.backgroundColor = option.palette.color( colorGroup, (option.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Background );

        QApplication::style()->drawPrimitive( QStyle::PE_FrameFocusRect, &o, painter );

    }

}

//_________________________________________________________
SimpleListView::SimpleListView( QWidget* parent ):
    TreeView( parent )
{
    setSortingEnabled( false );

    // replace item delegate
    if( itemDelegate() ) itemDelegate()->deleteLater();
    setItemDelegate( new SimpleListViewDelegate( this ) );

    // change font
    QFont boldFont( font() );
    boldFont.setBold( true );
    setFont( boldFont );
}

//_________________________________________________________
void SimpleListView::setModel( QAbstractItemModel* model )
{
    TreeView::setModel( model );
    if( model )
    {
        // show only first column and hide header
        setMask( 1 );
        header()->hide();

        // connect model modification to resize
        connect( model, SIGNAL(layoutChanged()), this, SLOT(_adjustWidth()) );
        _adjustWidth();
    }
}

//_______________________________________________
void SimpleListView::_adjustWidth()
{
    if( model() )
    { setFixedWidth( sizeHintForColumn(0) + 25 ); }
}
