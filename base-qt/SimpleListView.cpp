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
Private::SimpleListViewDelegate::SimpleListViewDelegate( QObject *parent ):
    QAbstractItemDelegate( parent ),
    Counter( "Private::SimpleListViewDelegate" )
{ Debug::Throw( "Private::SimpleListViewDelegate::SimpleListViewDelegate.\n" ); }

//_________________________________________________________
void Private::SimpleListViewDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    // check index
    if ( !index.isValid() ) return;

    QStyle* style( QApplication::style() );
    const int iconSize = style->pixelMetric(QStyle::PM_IconViewIconSize);

    const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
    const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const CustomPixmap pixmap( icon.pixmap( iconSize, iconSize ) );
    const QPen oldPen = painter->pen();

    auto fontMetrics = painter->fontMetrics();
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
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    const QColor color( qvariant_cast<QColor>(index.model()->data( index, Qt::ForegroundRole )) );
    if( color.isValid() )
    {

        painter->setPen( color );

    } else if ( option.state & QStyle::State_Selected ) {

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
QSize Private::SimpleListViewDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    if ( !index.isValid() ) return QSize();

    const int iconSize = QApplication::style()->pixelMetric(QStyle::PM_IconViewIconSize);
    auto text = index.model()->data( index, Qt::DisplayRole ).toString();
    auto icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const CustomPixmap pixmap( icon.pixmap( iconSize, iconSize ) );

    auto fontMetrics = option.fontMetrics;
    int gap = fontMetrics.height();
    int pixmapHeight = 0;
    int pixmapWidth = iconSize;

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

    width = qMax( textWidth, pixmapWidth ) + gap;
    width = qMax( width, fontMetrics.width( QString( 10, 'X' ) ) );

    return QSize( width, height );
}

//_________________________________________________________
int Private::SimpleListViewDelegate::_layoutText(QTextLayout *layout, int maxWidth) const
{
    qreal height = 0;
    int textWidth = 0;
    layout->beginLayout();
    forever
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
void Private::SimpleListViewDelegate::_drawFocus( QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const
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
    QListView( parent )
{
    // replace item delegate
    if( itemDelegate() ) itemDelegate()->deleteLater();
    setItemDelegate( new Private::SimpleListViewDelegate( this ) );
    setMouseTracking( true );
    setSelectionBehavior( SelectRows );

    // hover
    connect( this, SIGNAL(entered(QModelIndex)), SLOT(_indexEntered(QModelIndex)) );
}

//_________________________________________________________
void SimpleListView::setOrientation( Qt::Orientation value )
{
    if( orientation_ == value ) return;

    orientation_ = value;

    // unset previously set fixed dimension
    if( orientation_ == Qt::Vertical )
    {
        setMinimumHeight( 0 );
        setMaximumHeight( QWIDGETSIZE_MAX );
        setFlow( QListView::TopToBottom );
    } else {
        setMinimumWidth( 0 );
        setMaximumWidth( QWIDGETSIZE_MAX );
        setFlow( QListView::LeftToRight );
    }

    _adjustSize();
}

//_________________________________________________________
void SimpleListView::setModel( QAbstractItemModel* model )
{
    QListView::setModel( model );
    if( model )
    {
        // connect model modification to resize
        connect( model, SIGNAL(layoutChanged()), this, SLOT(_adjustSize()) );
        _adjustSize();
    }
}

//_______________________________________________
void SimpleListView::_adjustSize()
{
    if( !model() ) return;

    int rows = model()->rowCount();

    if( orientation_ == Qt::Vertical )
    {

        int width = 0;
        for( int i = 0; i < rows; ++i )
        { width = qMax(width, sizeHintForIndex(model()->index(i, 0)).width()); }

        setFixedWidth(width + 25);

    } else {

        int height = 0;
        for( int i = 0; i < rows; ++i )
        { height = qMax(height, sizeHintForIndex(model()->index(i, 0)).height()); }

        setFixedHeight(height + 10);

    }

}

//____________________________________________________________________________
bool SimpleListView::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::Leave:
        case QEvent::HoverLeave:
        {
            _setHoverIndex( QModelIndex() );
            break;
        }

        default: break;
    }

    return QListView::event( event );

}

//____________________________________________________________________
void SimpleListView::mouseMoveEvent( QMouseEvent *event )
{

    if( !indexAt( event->pos() ).isValid() )
    { _setHoverIndex( QModelIndex() ); }

    return QListView::mouseMoveEvent( event );
}

//__________________________________________________________
void SimpleListView::mousePressEvent( QMouseEvent* event )
{
    // clear hover index
    _setHoverIndex( QModelIndex() );

    if( (event->button() == Qt::RightButton) && selectionModel() && !indexAt( event->pos() ).isValid() )
    { selectionModel()->clear(); }

    return QListView::mousePressEvent( event );
}

//____________________________________________________________________
void SimpleListView::_indexEntered( const QModelIndex& index )
{ if( updatesEnabled() ) _setHoverIndex( index ); }

//____________________________________________________________________
void SimpleListView::_setHoverIndex( const QModelIndex& index )
{

    if( hoverIndex_ == index ) return;
    hoverIndex_ = index;

    // emit signal
    emit hovered( index );

}
