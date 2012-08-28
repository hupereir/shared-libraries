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

#include "IconView.h"
#include "CustomPixmap.h"
#include "Debug.h"
#include "Singleton.h"
#include "ScrollObject.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QDrag>
#include <QtGui/QPaintEvent>
#include <QtGui/QStyle>
#include <QtGui/QTextLayout>

//____________________________________________________________________
IconView::IconView( QWidget* parent ):
    QAbstractItemView( parent ),
    Counter( "IconView" ),
    pixmapSize_( 48, 48 ),
    margin_( 5 ),
    spacing_( 5 ),
    columnCount_( 1 ),
    rowCount_( 1 ),
    rubberBand_(0),
    dragButton_( Qt::NoButton )
{
    Debug::Throw( "IconView::IconView.\n" );
    header_ = new QHeaderView( Qt::Horizontal, this );
    header_->hide();

    connect( header_, SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( sortByColumn( int, Qt::SortOrder ) ) );
    connect( header_, SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( saveSortOrder( void ) ) );

    // enable sorting by default
    setSortingEnabled( true );
    verticalScrollBar()->adjustSize();
    setMouseTracking( true );

    new ScrollObject( this );

    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    _updateConfiguration();

}

//_______________________________________________
bool IconView::setOptionName( const QString& value )
{

    Debug::Throw() << "IconView::setOptionName - value: " << value << endl;

    QString tmp;

    // sort order
    bool sortChanged( false );
    tmp = value + "_SORT_ORDER";
    if( sortOrderOptionName_ != tmp  )
    {
        sortOrderOptionName_ = tmp;
        sortChanged = true;
    }

    // sort column
    tmp = value + "_SORT_COLUMN";
    if( sortColumnOptionName_ != tmp  )
    {

        sortColumnOptionName_ = tmp;
        sortChanged = true;

    }

    // reload sorting
    if( sortChanged )
    {
        if( !( XmlOptions::get().contains( sortOrderOptionName() ) && XmlOptions::get().contains( sortColumnOptionName() ) ) ) saveSortOrder();
        else updateSortOrder();
    }

    return sortChanged;

}

//____________________________________________________________________
QModelIndex IconView::indexAt( const QPoint& constPosition ) const
{

    // Transform the view coordinates into contents widget coordinates.
    QPoint position( constPosition + _scrollBarPosition() );

    // find matching item
    for( Item::Map::const_iterator iter = items_.begin(); iter != items_.end(); ++iter )
    {
        if( iter.value().boundingRect().translated( iter.value().position() ).contains( position ) )
        { return model()->index( iter.key(), 0 ); }
    }

    return QModelIndex();
}

//____________________________________________________________________
void IconView::scrollTo( const QModelIndex& index, ScrollHint )
{
    QRect area = viewport()->rect();
    QRect rect = visualRect(index);

    if( rect.left() < area.left() )
    {
        horizontalScrollBar()->setValue( horizontalScrollBar()->value() + rect.left() - area.left());

    } else if( rect.right() > area.right() ) {

        horizontalScrollBar()->setValue( horizontalScrollBar()->value() + qMin( rect.right() - area.right(), rect.left() - area.left()));

    }

    if (rect.top() < area.top())
    {

        verticalScrollBar()->setValue( verticalScrollBar()->value() + rect.top() - area.top());

    } else if (rect.bottom() > area.bottom()) {

        verticalScrollBar()->setValue( verticalScrollBar()->value() + qMin( rect.bottom() - area.bottom(), rect.top() - area.top()));

    }

    update();

}


//____________________________________________________________________
QRect IconView::visualRect( const QModelIndex&  index ) const
{

    if( items_.contains( index.row() ) )
    {
        const Item& item( items_[index.row()] );
        return item.boundingRect().translated( item.position() ).translated( -_scrollBarPosition() );
    } else return QRect();
}

//_____________________________________________________________________
void IconView::updateSortOrder( void )
{

    Debug::Throw( "IconView::updateSortOrder.\n" );
    if( !hasOptionName() ) return;
    if( XmlOptions::get().contains( sortColumnOptionName() ) && XmlOptions::get().contains( sortColumnOptionName() ) )
    {
        const bool changed(
            header()->sortIndicatorSection() != XmlOptions::get().get<int>( sortColumnOptionName() ) ||
            header()->sortIndicatorOrder() != XmlOptions::get().get<int>( sortOrderOptionName() ) );

        if( changed )
        { header_->setSortIndicator( XmlOptions::get().get<int>( sortColumnOptionName() ), (Qt::SortOrder)(XmlOptions::get().get<int>( sortOrderOptionName() ) ) ); }

    }

}

//_____________________________________________________________________
void IconView::saveSortOrder( void )
{

    Debug::Throw( "IconView::saveSortOrder.\n" );

    // save option
    if( !hasOptionName() ) return;
    XmlOptions::get().set<int>( sortOrderOptionName(), header()->sortIndicatorOrder() );
    XmlOptions::get().set<int>( sortColumnOptionName(), header()->sortIndicatorSection() );

}

//____________________________________________________________________
void IconView::doItemsLayout( void )
{

    const int rowCount( model()->rowCount() );

    // first remove invalid items
    const QList<int> keys( items_.keys() );
    foreach( const int& key, keys )
    { if( key >= rowCount ) items_.remove( key ); }

    // update existing items and insert new ones
    for( int row = 0; row < model()->rowCount(); ++row )
    {

        // index
        const QModelIndex index( model()->index( row, 0, QModelIndex() ) );

        // make sure row is in map
        if( items_.contains( row ) ) _updateItem( items_[row], index );
        else {

            Item item;
             _updateItem( item, index );
            items_.insert( row, item );

        }

    }

    _layoutItems();
    updateGeometries();
    viewport()->update();

}

//____________________________________________________________________
void IconView::updateGeometries( void )
{

    // vertical scrollbar
    verticalScrollBar()->setRange(0, qMax(0, boundingRect_.bottom() - viewport()->height()));
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setSingleStep( rowCount_ > 0 ? boundingRect_.height()/rowCount_ : viewport()->height()*0.1 );

    // horizontal scrollbar
    horizontalScrollBar()->setRange(0, qMax(0, boundingRect_.right() - viewport()->width()));
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setSingleStep( viewport()->width()*0.1 );

    QAbstractItemView::updateGeometries();

}

//____________________________________________________________________
void IconView::sortByColumn( int column, Qt::SortOrder order)
{
    Debug::Throw() << "IconView::sortByColumn - column: " << column << " order: " << order << endl;
    if( model() ) model()->sort( column, order );
}

//_____________________________________________________________________
void IconView::_updateConfiguration( void )
{
    Debug::Throw( "IconView::_updateConfiguration.\n" );

    // update sort order from options
    updateSortOrder();

    // update pixmap size
    int pixmapSize( 0 );
    if( XmlOptions::get().contains( "ICON_VIEW_PIXMAP_SIZE" ) && (pixmapSize = XmlOptions::get().get<int>( "ICON_VIEW_PIXMAP_SIZE" )) != pixmapSize_.width() )
    {
        pixmapSize_ = QSize( pixmapSize, pixmapSize );
        if( model() ) doItemsLayout();
    }
}

//____________________________________________________________________
bool IconView::isIndexHidden( const QModelIndex& ) const
{ return false; }

//____________________________________________________________________
QModelIndex IconView::moveCursor( CursorAction action, Qt::KeyboardModifiers )
{
    const QModelIndex index = currentIndex();
    if( !( index.isValid() && items_.contains( index.row() ) ) ) return QModelIndex();

    // get current item
    const Item& item( items_[index.row()] );

    switch( action )
    {
        case MoveLeft: return model()->index( item.row()*columnCount_ + item.column() - 1, 0 );
        case MoveRight: return model()->index( item.row()*columnCount_ + item.column() + 1, 0 );
        case MoveUp: return model()->index( (item.row()-1)*columnCount_ + item.column(), 0 );
        case MoveDown:
        {
            int row( (item.row()+1)*columnCount_ + item.column() );
            if( row < items_.size() ) return model()->index( row, 0 );
            else if( items_.values().back().row() > item.row() ) return model()->index( items_.size()-1, 0 );
            else return QModelIndex();
        }
        default: return QModelIndex();
    }

}

//____________________________________________________________________
void IconView::setSelection( const QRect& constRect, QItemSelectionModel::SelectionFlags flags )
{

    // translate
    const QRect rect( constRect.translated( _scrollBarPosition() ) );

    QModelIndexList indexes;
    for( Item::Map::const_iterator iter = items_.begin(); iter != items_.end(); ++iter )
    {

        const Item& item( iter.value() );
        if( rect.intersects( item.boundingRect().translated( item.position() ) ) )
        { indexes << model()->index( iter.key(), 0 ); }

    }

    if( indexes.empty() ) selectionModel()->select( QItemSelection(), flags );
    else {

        QItemSelection selection;
        foreach( const QModelIndex& index, indexes ) selection.select( index, index );
        selectionModel()->select( selection, flags );

    }

}

//____________________________________________________________________
void IconView::startDrag( Qt::DropActions supportedActions )
{

    Debug::Throw( "IconView::startDrag.\n" );

    // get list of dragable indexes
    QModelIndexList indexes;
    foreach( const QModelIndex& index, selectionModel()->selectedIndexes() )
    { if( model()->flags( index ) & Qt::ItemIsDragEnabled ) indexes << index; }
    if( indexes.isEmpty() ) return;

    // get mime data
    QMimeData* data = model()->mimeData( indexes );
    if( !data ) return;

    // create drag pixmap
    QRect rect;
    const QPixmap pixmap( _pixmap( indexes, rect ) );

    // create drag
    QDrag *drag = new QDrag(this);
    drag->setMimeData(data);
    drag->setPixmap( pixmap );

    drag->setHotSpot( dragOrigin_-rect.topLeft()+_scrollBarPosition());
    drag->exec( supportedActions, defaultDropAction() );

}

//____________________________________________________________________
void IconView::scrollContentsBy( int dx, int dy )
{
    dragOrigin_ += QPoint( dx, dy );
    QAbstractItemView::scrollContentsBy( dx, dy );
}

//____________________________________________________________________
int IconView::horizontalOffset( void ) const
{ return horizontalScrollBar()->value(); }

//____________________________________________________________________
int IconView::verticalOffset( void ) const
{ return verticalScrollBar()->value(); }

//____________________________________________________________________
QRegion IconView::visualRegionForSelection( const QItemSelection& selection ) const
{

    if( selection.empty() ) return QRegion();

    QRegion region;
    foreach( const QItemSelectionRange& range, selection )
    {
        foreach( const QModelIndex& index, range.indexes() )
        { region += visualRect( index ); }
    }

    return region;

}

//____________________________________________________________________
void IconView::paintEvent( QPaintEvent* event )
{


    QPainter painter( viewport() );
    painter.setClipRegion( event->region() );
    painter.translate( -_scrollBarPosition() );
    const QRect clipRect( event->rect().translated( _scrollBarPosition() ) );
    painter.setFont( QApplication::font() );
    painter.setRenderHint( QPainter::TextAntialiasing, true );

    // loop over events
    for( Item::Map::const_iterator iter = items_.begin(); iter != items_.end(); ++iter )
    {
        // check intersection with clipRect
        const Item& item( iter.value() );

        // todo: try understand offsets properly
        if( !item.boundingRect().translated( item.position() ).intersects( clipRect ) ) continue;

        // setup option
        QStyleOptionViewItemV4 option = viewOptions();
        option.rect = item.boundingRect();

        const QModelIndex index( model()->index( iter.key(), 0 ) );
        if( selectionModel()->isSelected( index ) )
        {
            option.viewItemPosition = QStyleOptionViewItemV4::OnlyOne;
            option.state |= QStyle::State_Selected;
        }

        if( index == hoverIndex_ ) option.state |= QStyle::State_MouseOver;

        painter.translate( item.position() );
        item.paint( &painter, &option, this );
        painter.translate( -item.position() );
    }

}

//____________________________________________________________________
void IconView::resizeEvent( QResizeEvent* event )
{

    QAbstractItemView::resizeEvent( event );
    _layoutItems();

}

//____________________________________________________________________
void IconView::mousePressEvent( QMouseEvent* event )
{

    QAbstractItemView::mousePressEvent( event );

    // clear hover index
    _setHoverIndex( QModelIndex() );

    // store button and position
    dragButton_ = event->button();
    dragOrigin_ = event->pos();

    // nothing if on item
    if( indexAt( event->pos() ).isValid() ) return;

    // prepare rubberband
    if( dragButton_ == Qt::LeftButton )
    {
        selectionModel()->clear();
        if( !rubberBand_ ) rubberBand_ = new QRubberBand(QRubberBand::Rectangle, viewport());
        rubberBand_->setGeometry( QRect( dragOrigin_, QSize() ) );
        rubberBand_->show();
    }

}

//____________________________________________________________________
void IconView::mouseMoveEvent(QMouseEvent *event)
{

    // update rubber band
    if( dragButton_ == Qt::LeftButton && rubberBand_ && rubberBand_->isVisible() )
    {

        _setHoverIndex( QModelIndex() );

        rubberBand_->setGeometry(QRect( dragOrigin_, event->pos() ).normalized() );
        if( autoScrollTimer_.isActive())
        {
            if( viewport()->rect().contains( event->pos() ) ) autoScrollTimer_.stop();
        } else if (!viewport()->rect().contains( event->pos() )) autoScrollTimer_.start(100, this);

    } else {

        _setHoverIndex( indexAt( event->pos() ) );

    }

    QAbstractItemView::mouseMoveEvent(event);


}

//____________________________________________________________________
void IconView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);

    autoScrollTimer_.stop();

    // check rubber band
    if( dragButton_ == Qt::LeftButton && rubberBand_ )
    {
        rubberBand_->hide();
        viewport()->update();
    }

    dragButton_ = Qt::NoButton;
}

//____________________________________________________________________
void IconView::dragMoveEvent(QDragMoveEvent *event)
{

    // update hover item
    if( showDropIndicator() ) _setHoverIndex( indexAt( event->pos() ) );
    else _setHoverIndex( QModelIndex() );

    // parent class
    QAbstractItemView::dragMoveEvent(event);
}

//______________________________________________________________
void IconView::timerEvent(QTimerEvent *event)
{

    if( event->timerId() == autoScrollTimer_.timerId() )
    {
        const QPoint globalPosition = QCursor::pos();
        const QPoint position = viewport()->mapFromGlobal(globalPosition);
        const QRect rect( viewport()->rect() );

        if( position.y() < 0 ) verticalScrollBar()->setValue( verticalScrollBar()->value() - verticalScrollBar()->singleStep() );
        else if( position.y() > rect.height() ) verticalScrollBar()->setValue( verticalScrollBar()->value() + verticalScrollBar()->singleStep() );

    } else return QAbstractItemView::timerEvent( event );

}

//____________________________________________________________________
void IconView::_updateItem( Item& item, const QModelIndex& index ) const
{

    // update text
    QVariant textVariant( model()->data( index, Qt::DisplayRole ) );
    if( textVariant.canConvert( QVariant::String ) ) item.setText( textVariant.value<QString>() );

    // update pixmap
    QVariant iconVariant( model()->data( index, Qt::DecorationRole ) );
    if( iconVariant.canConvert( QVariant::Icon ) ) item.setPixmap( iconVariant.value<QIcon>().pixmap( pixmapSize_ ) );

}

//____________________________________________________________________
void IconView::_layoutItems( void )
{

    // get max width for items
    const int maxWidth( width() - 2*margin_ - verticalScrollBar()->width() );
    const int maxHeight( height() - 2*margin_ );
    int width( 0 );
    columnCount_ = 0;

    for( Item::Map::const_iterator iter = items_.begin(); iter != items_.end(); ++iter, ++columnCount_ )
    {
        if( columnCount_ ) width += spacing_;
        const Item& item( iter.value() );
        width += item.boundingRect().width();
        if( width > maxWidth ) break;
    }

    // make sure to use at least one column
    columnCount_ = qMax( columnCount_, 1 );

    // calculate column sizes
    rowCount_ = 0;
    int totalHeight( 0 );
    int totalWidth( 0 );
    QVector<int> columnSizes;
    while( columnCount_ >= 1 )
    {

        int rowHeight( 0 );
        int column( 0 );
        rowCount_ = 0;
        totalHeight = 0;
        columnSizes = QVector<int>( columnCount_, 0 );
        for( Item::Map::const_iterator iter = items_.begin(); iter != items_.end(); ++iter, ++column )
        {
            // reset column
            const Item& item( iter.value() );
            if( column >= columnCount_ )
            {
                if( totalHeight ) totalHeight += spacing_;
                totalHeight += rowHeight;
                rowHeight = 0;
                column = 0;
                ++rowCount_;
            }

            rowHeight = qMax( rowHeight, item.boundingRect().height() );
            columnSizes[column] = qMax(columnSizes[column], item.boundingRect().width() );

        }

        // account for last row
        totalHeight += rowHeight;

        // compute total width and compare to max
        totalWidth = 0;
        foreach( const int& width, columnSizes ) totalWidth += width;
        totalWidth += (columnSizes.size()-1)*spacing_;

        if( totalWidth <= maxWidth || columnCount_ == 1 ) break;
        columnCount_--;
    }

    // evenly distribute extra width if there is more than one row
    int extraWidth( 0 );
    if( rowCount_ > 0 )
    {
        extraWidth = maxWidth - totalWidth;
        if( totalHeight < maxHeight ) extraWidth += verticalScrollBar()->width();
        extraWidth = qMax( 0, extraWidth/(columnCount_+1) );
    }

    const int margin = margin_ + extraWidth;
    const int spacing = spacing_ + extraWidth;

    // layout items
    int rowHeight( 0 );
    int column = 0;
    int row = 0;
    boundingRect_ = QRect();
    QPoint position( margin, margin_ );
    for( Item::Map::iterator iter = items_.begin(); iter != items_.end(); ++iter, ++column )
    {

        Item& item( iter.value() );

        // reset column
        if( column >= columnCount_ )
        {
            column = 0;
            ++row;
            position = QPoint( margin, position.y() + rowHeight + spacing_ );
            rowHeight = 0;
        }

        item.setPosition( position + QPoint( ( columnSizes[column] - item.boundingRect().width() )/2, 0 ) );
        item.setLocation( row, column );
        boundingRect_ |= item.boundingRect().translated( item.position() );
        rowHeight = qMax( rowHeight, item.boundingRect().height() );
        position = QPoint( position.x() + columnSizes[column] + spacing, position.y() );
    }

}

//____________________________________________________________________
QPixmap IconView::_pixmap( const QModelIndexList& indexes, QRect& boundingRect )
{
    if( indexes.isEmpty() ) return QPixmap();

    Item::List items;
    foreach( const QModelIndex& index, indexes )
    {
        if( !items_.contains( index.row() ) ) continue;
        items << items_[index.row()];
        boundingRect |= items.back().boundingRect().translated( items.back().position() );
    }

    // create pixmap
    QPixmap pixmap( boundingRect.size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.translate( -boundingRect.topLeft() );
    foreach( const Item& item, items )
    {

        QStyleOptionViewItemV4 option = viewOptions();
        option.rect = item.boundingRect();

        // assume all indexes are selected
        option.viewItemPosition = QStyleOptionViewItemV4::OnlyOne;
        option.state |= QStyle::State_Selected;

        painter.translate( item.position() );
        item.paint( &painter, &option, this );
        painter.translate( -item.position() );

    }

    return pixmap;

}


//____________________________________________________________________
QRect IconView::Item::boundingRect( void ) const
{
    if( dirty_ ) const_cast<IconView::Item*>( this )->_updateBoundingRect();
    return boundingRect_;
}

//____________________________________________________________________
void IconView::Item::paint( QPainter* painter, const QStyleOption* option, QWidget* widget ) const
{

    QRectF boundingRect( this->boundingRect() );
    QRectF textRect( boundingRect.adjusted( 0, margin_, 0, -margin_ ) );

    // draw selection
    widget->style()->drawPrimitive( QStyle::PE_PanelItemViewItem, option, painter, widget );

    if( !pixmap_.isNull() )
    {
        painter->drawPixmap( QPointF( (boundingRect.width() - pixmap_.width())/2, margin_ ), pixmap_ );
        textRect.adjust( 0, pixmap_.height() + spacing_, 0, 0 );
    }

    if( !text_.isEmpty() )
    {

        const int maxWidth( qMax( maxTextWidth_, pixmap_.width() ) );
        QTextOption textOption(Qt::AlignHCenter);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        qreal height(0);
        QTextLine line;
        QTextLayout layout( text_, QApplication::font() );
        layout.setTextOption(textOption);
        layout.beginLayout();
        while( ( line = layout.createLine() ).isValid())
        {

            line.setLineWidth(maxWidth);
            line.naturalTextWidth();
            line.setPosition(QPointF(0, height));
            height += line.height();

        }

        layout.endLayout();
        layout.draw( painter, textRect.topLeft() + QPointF( 0.5*(textRect.width()-layout.boundingRect().width()), 0 ), QVector<QTextLayout::FormatRange>(), textRect );

    }

}

//____________________________________________________________________
void IconView::Item::_updateBoundingRect( void )
{
    boundingRect_ = QRect( 0, 0, 2*margin_, 2*margin_ );

    // calculate pixmap size
    QSize pixmapSize( pixmap_.size() );
    // calculate text size
    QSize textSize;
    if( !text_.isEmpty() )
    {

        const int maxWidth( qMax( maxTextWidth_, pixmapSize.width() ) );

        QTextOption textOption(Qt::AlignHCenter);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        qreal height(0);

        QTextLine line;
        QTextLayout layout( text_, QApplication::font() );
        layout.setTextOption(textOption);
        layout.beginLayout();
        while( ( line = layout.createLine() ).isValid())
        {

            line.setLineWidth(maxWidth);
            line.naturalTextWidth();
            line.setPosition( QPointF( 0, height ) );
            height += line.height();

        }

        layout.endLayout();
        textSize = layout.boundingRect().size().toSize();

    }

    if( !( pixmap_.isNull() || text_.isEmpty() ) )
    {

        boundingRect_.adjust( 0, 0, qMax( pixmapSize.width(), textSize.width() ), spacing_ + pixmapSize.height() + textSize.height() );

    } else if( !pixmap_.isNull() ) {

        boundingRect_.adjust( 0, 0, pixmapSize.width(), pixmapSize.height() );

    } else if( !text_.isEmpty() ) {

        boundingRect_.adjust( 0, 0, textSize.width(), textSize.height() );

    }

    dirty_ = false;
}
