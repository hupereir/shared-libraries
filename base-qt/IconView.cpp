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

#include "IconView.h"

#include "BaseFindDialog.h"
#include "BaseFindWidget.h"
#include "BaseIconNames.h"
#include "CppUtil.h"
#include "Debug.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "InformationDialog.h"
#include "ItemModel.h"
#include "RegExpUtil.h"
#include "Singleton.h"
#include "TextEditor.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QDrag>
#include <QHoverEvent>
#include <QPaintEvent>
#include <QRegularExpression>
#include <QStyle>

#include <numeric>

//____________________________________________________________________
IconView::IconView( QWidget* parent ):
    QAbstractItemView( parent ),
    Counter( "IconView" )
{
    Debug::Throw( "IconView::IconView.\n" );

    iconSizes_ =
    {
        IconSize::Size::Small,
        IconSize::Size::Medium,
        IconSize::Size::Large,
        IconSize::Size::VeryLarge,
        IconSize::Size::Huge,
        80,
        96,
        112,
        IconSize::Size::VeryHuge,
        144,
        160,
        176,
        192,
        208,
        224,
        240,
        IconSize::Size::Maximum
    };

    // actions
    _installActions();

    // header (needed for sorting, even though always hidden)
    header_ = new QHeaderView( Qt::Horizontal, this );
    header_->hide();

    connect( header_, &QHeaderView::sortIndicatorChanged, this, &IconView::sortByColumn );
    connect( header_, &QHeaderView::sortIndicatorChanged, this, &IconView::saveSortOrder );

    // enable sorting by default
    setSortingEnabled( true );
    setSelectionBehavior( SelectRows );
    verticalScrollBar()->adjustSize();
    horizontalScrollBar()->adjustSize();
    setMouseTracking( true );

    connect( verticalScrollBar(), &QAbstractSlider::valueChanged, this, &IconView::_updateHoverIndex );
    connect( horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &IconView::_updateHoverIndex );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &IconView::_updateConfiguration );
    _updateConfiguration();
}

//_______________________________________________
IconView::~IconView() = default;

//_______________________________________________
void IconView::setModel( QAbstractItemModel* model )
{
    Debug::Throw( "IconView::setModel.\n" );
    QAbstractItemView::setModel( model );
    header_->setModel( model );

    if( (model_ = qobject_cast<ItemModel*>(model)) )
    {
        connect( model_, &QAbstractItemModel::layoutAboutToBeChanged, this, &IconView::saveSelectedIndexes );
        connect( model_, &QAbstractItemModel::layoutChanged, this, &IconView::restoreSelectedIndexes );
    }
}

//______________________________________________________________________
void IconView::setFindWidget( AbstractFindWidget* widget )
{

    Debug::Throw( "IconView::setFindWidget.\n" );

    if( findWidget_ ) findWidget_->deleteLater();
    findWidget_ = widget;

    // connections
    connect( findWidget_, &AbstractFindWidget::find, this, &IconView::find );
    connect( this, &IconView::matchFound, findWidget_, &AbstractFindWidget::matchFound );
    connect( this, &IconView::noMatchFound, findWidget_, &AbstractFindWidget::noMatchFound );

}

//_______________________________________________
void IconView::setFindEnabled( bool value )
{
    Debug::Throw( "IconView::setFindEnabled.\n" );
    findAction_->setEnabled( value );
    findSelectionAction_->setEnabled( value );
    findAgainAction_->setEnabled( value );
    findSelectionBackwardAction_->setEnabled( value );
    findAgainBackwardAction_->setEnabled( value );
}

//______________________________________________________________________
TextSelection IconView::selection() const
{

    Debug::Throw( "IconView::selection.\n" );

    // copy last selection
    TextSelection out;
    out.setFlag( TextSelection::CaseSensitive, TextEditor::lastSelection().flag( TextSelection::CaseSensitive ) );
    out.setFlag( TextSelection::EntireWord, TextEditor::lastSelection().flag( TextSelection::EntireWord ) );

    QString text;
    if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) out.setText( text );
    else if( selectionModel() && model_ && selectionModel()->currentIndex().isValid() )
    {
        const auto current( selectionModel()->currentIndex() );
        if( !(text =  model_->data( current ).toString()).isEmpty() ) out.setText( text );
    }

    // if everything else failed, retrieve last selection
    if( text.isEmpty() ) out.setText( TextEditor::lastSelection().text() );

    return out;

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
    for( auto&& iter = items_.begin(); iter != items_.end(); ++iter )
    {
        if( iter.value().boundingRect().translated( iter.value().position() ).contains( position ) )
        { return model_->index( iter.key(), 0 ); }
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
    auto iter = items_.find( index.row() );
    if( iter != items_.end() )
    {
        const auto& item( iter.value() );
        return item.boundingRect().translated( item.position() ).translated( -_scrollBarPosition() );
    } else return QRect();
}

//_____________________________________________________________________
QSize IconView::minimumSizeHint() const
{
    return QSize(
        2*margin_ + 2*IconViewItem::margin + qMax( IconViewItem::maxTextWidth, iconSize().width() ) + verticalScrollBar()->width(),
        QAbstractItemView::minimumSizeHint().height() );
}

//_____________________________________________________________________
void IconView::updateSortOrder()
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
void IconView::saveSortOrder()
{

    Debug::Throw( "IconView::saveSortOrder.\n" );

    // save option
    if( !hasOptionName() ) return;
    XmlOptions::get().set<int>( sortOrderOptionName(), header()->sortIndicatorOrder() );
    XmlOptions::get().set<int>( sortColumnOptionName(), header()->sortIndicatorSection() );

}

//____________________________________________________________________
void IconView::doItemsLayout()
{

    const int rowCount( model_->rowCount() );

    // first remove invalid items
    for( const auto& key:items_.keys() )
    { if( key >= rowCount ) items_.remove( key ); }

    // update existing items and insert new ones
    for( int row = 0; row < rowCount; ++row )
    {

        // index
        const auto index( model_->index( row, 0 ) );

        // make sure row is in map
        auto iter( items_.lowerBound( row ) );
        if( iter != items_.end() && Base::areEquivalent( row, iter.key() ) )
        {

            _updateItem( iter.value(), index );

        } else {

            IconViewItem item;
             _updateItem( item, index );
            Base::insert( items_, iter, row, item );
        }

    }

    _layoutItems();
    updateGeometries();
    viewport()->update();

}

//______________________________________________________________________
void IconView::find( TextSelection selection )
{
    Debug::Throw( "IconView::find.\n" );
    bool found( selection.flag( TextSelection::Backward ) ? _findBackward( selection, true ):_findForward( selection, true ) );
    if( found ) emit matchFound();
    else emit noMatchFound();
}

//______________________________________________________________________
void IconView::findAgainForward()
{ _findForward( TextEditor::lastSelection(), true ); }

//______________________________________________________________________
void IconView::findAgainBackward()
{ _findBackward( TextEditor::lastSelection(), true ); }

//__________________________________________________________
void IconView::saveSelectedIndexes()
{
    if( selectionModel() )
    {
        model_->setSelectedIndexes( selectionModel()->selectedRows() );
        model_->setCurrentIndex( selectionModel()->currentIndex() );
    }
}

//__________________________________________________________
void IconView::restoreSelectedIndexes()
{

    if( selectionModel() )
    {

        const auto selection( model_->selectedIndexes() );
        selectionModel()->clear();
        for( const auto& index:selection )
        { selectionModel()->select( index, QItemSelectionModel::Select|QItemSelectionModel::Rows ); }

        selectionModel()->setCurrentIndex( model_->currentIndex(), QItemSelectionModel::Current );

    }

    return;
}

//____________________________________________________________________
bool IconView::isIndexHidden( const QModelIndex& ) const
{ return false; }

//____________________________________________________________________
QModelIndex IconView::moveCursor( CursorAction action, Qt::KeyboardModifiers )
{

    Debug::Throw() << "IconView::moveCursor - action: " << action << endl;

    // current index
    auto index = currentIndex();
    if( !index.isValid() ) index = model_->index( 0, 0 );
    if( !index.isValid() ) return QModelIndex();

    auto iter = items_.find( index.row() );
    if( iter == items_.end() ) return QModelIndex();

    // convert next/previous action depending on layout direction
    const bool isRightToLeft( qApp->isRightToLeft() );
    switch( action )
    {
        case MoveNext: action = isRightToLeft ? MoveRight:MoveLeft; break;
        case MovePrevious: action = isRightToLeft ? MoveLeft:MoveRight; break;
        default: break;
    }

    // get current item
    QModelIndex targetIndex;
    const auto& item( iter.value() );
    switch( action )
    {

        case MoveHome: targetIndex = model_->index( 0, 0 ); break;
        case MoveEnd: targetIndex = model_->index( items_.size()-1, 0 ); break;

        case MoveLeft: targetIndex = model_->index( item.row()*columnCount_ + item.column() - 1, 0 ); break;
        case MoveRight: targetIndex = model_->index( item.row()*columnCount_ + item.column() + 1, 0 ); break;
        case MoveUp: targetIndex = model_->index( (item.row()-1)*columnCount_ + item.column(), 0 ); break;
        case MoveDown:
        {
            int row( (item.row()+1)*columnCount_ + item.column() );
            if( row < items_.size() ) targetIndex = model_->index( row, 0 );
            else if( items_.values().back().row() > item.row() ) targetIndex = model_->index( items_.size()-1, 0 );
            else targetIndex = QModelIndex();
            break;
        }

        case MovePageUp:
        {
            const qreal target( item.position().y() + item.boundingRect().height() - viewport()->height() );
            for( int i = 0; i <= index.row(); ++i )
            {
                const auto& iter( items_.find(i) );
                if( iter == items_.end() ) continue;

                const auto& local( iter.value() );
                if( local.column() != item.column() ) continue;
                else if( local.position().y() > target )
                {

                    targetIndex = model_->index( i, 0 );
                    break;

                }

            }

            break;
        }

        case MovePageDown:
        {
            const qreal target( item.position().y() + viewport()->height() );
            for( int i = index.row()+1; i < items_.size(); ++i )
            {
                const auto& iter( items_.find(i) );
                if( iter == items_.end() ) continue;

                const auto& local( iter.value() );
                if( local.column() != item.column() ) continue;
                else if( !targetIndex.isValid() || local.position().y() + local.boundingRect().height() < target )
                {

                    targetIndex = model_->index( i, 0 );

                } else break;
            }

            break;

        }

        default: break;
    }

    // return result
    return targetIndex.isValid() ? targetIndex:index;

}

//____________________________________________________________________
void IconView::setSelection( const QRect& constRect, QItemSelectionModel::SelectionFlags flags )
{

    const auto indexes( _selectedIndexes( constRect ) );
    if( indexes.empty() ) selectionModel()->select( QItemSelection(), flags );
    else {

        QItemSelection selection;
        for( const auto& index:indexes ) selection.select( index, index );
        selectionModel()->select( selection, flags );

    }

}

//____________________________________________________________________
void IconView::startDrag( Qt::DropActions supportedActions )
{

    Debug::Throw( "IconView::startDrag.\n" );

    // get list of dragable indexes
    QModelIndexList indexes;
    for( const auto& index:selectionModel()->selectedIndexes() )
    { if( model_->flags( index ) & Qt::ItemIsDragEnabled ) indexes.append( index ); }
    if( indexes.isEmpty() ) return;

    // get mime data
    QMimeData* data = model_->mimeData( indexes );
    if( !data ) return;

    // reset hover index
    _setHoverIndex( QModelIndex() );

    // create drag pixmap
    QRect rect;
    const auto pixmap( _pixmap( indexes, rect ) );

    // create drag
    auto drag = new QDrag(this);
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
int IconView::horizontalOffset() const
{ return horizontalScrollBar()->value(); }

//____________________________________________________________________
int IconView::verticalOffset() const
{ return verticalScrollBar()->value(); }

//____________________________________________________________________
QRegion IconView::visualRegionForSelection( const QItemSelection& selection ) const
{

    if( selection.empty() ) return QRegion();

    auto indexes( selection.indexes() );
    return std::accumulate( indexes.begin(), indexes.end(), QRegion(),
        [this]( QRegion region, const QModelIndex& index )
        { return std::move(region) + visualRect( index ); } );

}

//____________________________________________________________________________
bool IconView::event( QEvent* event )
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

    return QAbstractItemView::event( event );

}

//____________________________________________________________________
void IconView::paintEvent( QPaintEvent* event )
{

    QPainter painter( viewport() );
    painter.setRenderHint( QPainter::TextAntialiasing, true );
    painter.setClipRegion( event->region() );
    painter.translate( -_scrollBarPosition() );
    const auto clipRect( event->rect().translated( _scrollBarPosition() ) );
    painter.setFont( QApplication::font() );
    painter.setRenderHint( QPainter::TextAntialiasing, true );

    // loop over events
    for( auto&& iter = items_.constBegin(); iter != items_.constEnd(); ++iter )
    {
        // check intersection with clipRect
        const auto& item( iter.value() );

        // todo: try understand offsets properly
        if( !item.boundingRect().translated( item.position() ).intersects( clipRect ) ) continue;

        // setup option
        const auto index( model_->index( iter.key(), 0 ) );
        auto option = _viewOptions( index );
        option.rect = item.boundingRect();

        // paint
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

//_____________________________________________________________________
void IconView::keyPressEvent( QKeyEvent* event )
{

    // get current index
    QModelIndex index;
    switch( event->key() )
    {

        case Qt::Key_Home: index = moveCursor( MoveHome, event->modifiers() ); break;
        case Qt::Key_End: index = moveCursor( MoveEnd, event->modifiers() ); break;
        case Qt::Key_Up: index = moveCursor( MoveUp, event->modifiers() ); break;
        case Qt::Key_Down: index = moveCursor( MoveDown, event->modifiers() ); break;
        case Qt::Key_Left: index = moveCursor( MoveLeft, event->modifiers() ); break;
        case Qt::Key_Right: index = moveCursor( MoveRight, event->modifiers() ); break;
        case Qt::Key_PageUp: index = moveCursor( MovePageUp, event->modifiers() ); break;
        case Qt::Key_PageDown: index = moveCursor( MovePageDown, event->modifiers() ); break;

        default: return QAbstractItemView::keyPressEvent( event );

    }


    // update anchor index if not set
    const auto oldIndex( currentIndex() );
    if( !( anchorIndex_.isValid() && selectionModel()->isSelected( anchorIndex_ ) ) && oldIndex.isValid() && selectionModel()->isSelected( oldIndex ) )
    { anchorIndex_ = oldIndex; }

    // get modifier
    const bool shiftPressed( event->modifiers() & Qt::ShiftModifier );
    if( !index.isValid() )
    {

        if( !shiftPressed )
        {
            anchorIndex_ = index;
            selectionModel()->clear();
        }

    } else if( !( shiftPressed && oldIndex.isValid() && anchorIndex_.isValid() ) ) {

        // update anchor index
        anchorIndex_ = index;

        // update selection
        selectionModel()->clear();
        selectionModel()->setCurrentIndex( index, QItemSelectionModel::Current );
        selectionModel()->select( index, QItemSelectionModel::Select|QItemSelectionModel::Rows );

    } else {

        // update selection
        selectionModel()->clear();
        selectionModel()->setCurrentIndex( index, QItemSelectionModel::Current );
        selectionModel()->select( QItemSelection( anchorIndex_, index ), QItemSelectionModel::Select|QItemSelectionModel::Rows );

    }

    return;

}

//____________________________________________________________________
void IconView::mousePressEvent( QMouseEvent* event )
{

    // clear hover index
    _setHoverIndex( QModelIndex() );

    // clear selection for right button click on invalid index
    if( (event->button() == Qt::RightButton) && selectionModel() && !indexAt( event->pos() ).isValid() )
    { selectionModel()->clear(); }

    // store button and position
    dragButton_ = event->button();
    dragOrigin_ = event->pos();

    // process non left button
    if( dragButton_ != Qt::LeftButton )
    { return QAbstractItemView::mousePressEvent( event ); }

    const bool shiftPressed( event->modifiers() & Qt::ShiftModifier );
    const auto index = indexAt( event->pos() );
    if( index.isValid() )
    {

        // drag
        if( selectionModel()->isSelected( index ) && dragEnabled() && !shiftPressed )
        {
            anchorIndex_ = index;
            return QAbstractItemView::mousePressEvent( event );
        }

        if( shiftPressed && anchorIndex_.isValid() && selectionModel()->isSelected( anchorIndex_ ) )
        {

            Debug::Throw( "IconView::mousePressEvent - using anchor widget.\n" );
            selectionModel()->clear();
            selectionModel()->setCurrentIndex( index, QItemSelectionModel::Current );
            selectionModel()->select( QItemSelection( anchorIndex_, index ), QItemSelectionModel::Select|QItemSelectionModel::Rows );
            setState( DragSelectingState );

        } else {

            anchorIndex_ = index;
            selectionModel()->clear();
            selectionModel()->setCurrentIndex( index, QItemSelectionModel::Current );
            selectionModel()->select( index, QItemSelectionModel::Select|QItemSelectionModel::Rows );

            // ensure proper state is set
            QAbstractItemView::mousePressEvent( event );
            setState( dragEnabled() ? DraggingState : DragSelectingState );
            return;

        }

    } else {

        // clear selection
        if( !shiftPressed )
        {
            selectionModel()->clear();
            anchorIndex_ = QModelIndex();
        }

        // default processing
        QAbstractItemView::mousePressEvent( event );
        setState( DragSelectingState );

    }


}

//____________________________________________________________________
void IconView::mouseMoveEvent( QMouseEvent *event )
{

    if( dragButton_ == Qt::LeftButton && ( event->buttons() & Qt::LeftButton ) && !( dragEnabled() && indexAt( dragOrigin_ ).isValid() ) )
    {

        if( !rubberBand_ ) rubberBand_ = new QRubberBand(QRubberBand::Rectangle, viewport());
        if( !rubberBand_->isVisible() ) rubberBand_->show();

        // disable hover index
        _setHoverIndex( QModelIndex() );

        // update rubber band
        rubberBand_->setGeometry(QRect( dragOrigin_, event->pos() ).normalized() );

        // autoscroll
        if( autoScrollTimer_.isActive())
        {
            if( viewport()->rect().contains( event->pos() ) ) autoScrollTimer_.stop();

        } else if (!viewport()->rect().contains( event->pos() )) autoScrollTimer_.start(100, this);

        selectionModel()->clear();
        setSelection( rubberBand_->geometry(), QItemSelectionModel::Select|QItemSelectionModel::Rows );


    } else {

        // default implementation
        _setHoverIndex( indexAt( event->pos() ) );

        if( event->buttons() & Qt::LeftButton )
        { QAbstractItemView::mouseMoveEvent(event); }

    }


}

//____________________________________________________________________
void IconView::mouseReleaseEvent( QMouseEvent *event )
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

//________________________________________________
void IconView::wheelEvent( QWheelEvent* event )
{

    if( event->modifiers() & Qt::ControlModifier )
    {

        event->accept();

        // calculate delta
        const auto offset = double(  event->delta() )/120;

        // check if direction has changed
        if( wheelOffsetAccumulated_ != 0 && Base::sign(offset) != Base::sign( wheelOffsetAccumulated_ ) )
        { wheelOffsetAccumulated_ = 0; }

        const auto offsetInt = int( wheelOffsetAccumulated_ + offset );
        if( offsetInt != 0 ) _incrementIconSize( offsetInt );
        wheelOffsetAccumulated_ += offset - offsetInt;

        return;

    } else {

        wheelOffsetAccumulated_ = 0;
        return QAbstractItemView::wheelEvent( event );

    }

}

//____________________________________________________________________
void IconView::dragEnterEvent( QDragEnterEvent *event )
{

    Debug::Throw() << "IconView::dragEnterEvent" << endl;

    if( acceptDrops() && event->isAccepted() )
    { dragInProgress_ = true; }

    // parent class
    QAbstractItemView::dragEnterEvent(event);

}

//____________________________________________________________________
void IconView::dragMoveEvent( QDragMoveEvent *event )
{

    if( acceptDrops() && event->isAccepted() )
    { dragInProgress_ = true; }

    Debug::Throw() << "IconView::dragMoveEvent" << endl;

    // update hover item
    if( showDropIndicator() ) _setHoverIndex( indexAt( event->pos() ) );
    else _setHoverIndex( QModelIndex() );

    // parent class
    QAbstractItemView::dragMoveEvent(event);
}

//____________________________________________________________________
void IconView::dragLeaveEvent( QDragLeaveEvent *event )
{
    Debug::Throw() << "IconView::dragLeaveEvent" << endl;
    dragInProgress_ = false;
    QAbstractItemView::dragLeaveEvent(event);
}

//____________________________________________________________________
void IconView::dropEvent( QDropEvent *event )
{
    dragInProgress_ = false;
    QAbstractItemView::dropEvent(event);
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
QModelIndexList IconView::_selectedIndexes( const QRect& constRect ) const
{

    QModelIndexList indexes;
    const QRect rect( constRect.translated( _scrollBarPosition() ) );
    for( auto&& iter = items_.constBegin(); iter != items_.constEnd(); ++iter )
    {

        const auto& item( iter.value() );
        if( rect.intersects( item.boundingRect().translated( item.position() ) ) )
        { indexes.append( model_->index( iter.key(), 0 ) ); }

    }

    return indexes;
}

//____________________________________________________________________
void IconView::_setHoverIndex( const QModelIndex& index )
{
    if( hoverIndex_ == index ) return;
    hoverIndex_ = index;

    // emit signal
    if( !( index.isValid() && dragInProgress_ ) )
    { emit hovered( index ); }

}

//____________________________________________________________________
void IconView::_updateItem( IconViewItem& item, const QModelIndex& index ) const
{

    // update text
    QVariant textVariant( model_->data( index, Qt::DisplayRole ) );
    if( textVariant.canConvert( QVariant::String ) ) item.setText( textVariant.value<QString>() );

    // update pixmap
    QVariant decorationVariant( model_->data( index, Qt::DecorationRole ) );
    if( decorationVariant.canConvert( QVariant::Icon ) ) item.setPixmap( CustomPixmap( decorationVariant.value<QIcon>().pixmap( iconSize() ) ) );
    else if( decorationVariant.canConvert( QVariant::Pixmap ) ) item.setPixmap( CustomPixmap( decorationVariant.value<QPixmap>() ) );

}

//____________________________________________________________________
void IconView::_layoutItems()
{

    // get max width for items
    const int maxWidth( width() - 2*margin_ - verticalScrollBar()->width() );
    const int maxHeight( height() - 2*margin_ );
    int width( 0 );
    columnCount_ = 0;

    // estimate number of columns based on first row
    for( auto&& iter = items_.constBegin(); iter != items_.constEnd() && width <= maxWidth; ++iter, ++columnCount_ )
    {
        const auto& item( iter.value() );
        if( columnCount_ > 0 ) width += spacing_;
        width += item.boundingRect().width();
    }

    // make sure to use at least one column
    columnCount_ = qMax( columnCount_, 1 );

    // calculate column sizes
    rowCount_ = 0;
    int totalHeight( 0 );
    int totalWidth( 0 );
    QVector<int> columnSizes;
    for( ; columnCount_ >= 1; columnCount_-- )
    {

        int rowHeight( 0 );
        int column( 0 );
        rowCount_ = 0;
        totalHeight = 0;
        columnSizes = QVector<int>( columnCount_, 0 );
        for( auto&& iter = items_.constBegin(); iter != items_.constEnd(); ++iter, ++column )
        {
            // reset column
            const auto& item( iter.value() );

            // start new row
            if( column >= columnCount_ )
            {
                if( totalHeight > 0 ) totalHeight += spacing_;
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
        totalWidth =
            std::accumulate( columnSizes.begin(), columnSizes.end(), 0 ) +
            (columnSizes.size()-1)*spacing_;

        if( totalWidth <= maxWidth ) break;
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
    for( auto&& iter = items_.begin(); iter != items_.end(); ++iter, ++column )
    {

        auto& item( iter.value() );

        // start a new row
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

    IconViewItem::Map items;
    for( const auto& index:indexes )
    {
        // find item
        auto iter( items_.find( index.row() ) );
        if( iter == items_.end() ) continue;

        // insert in map and update bounding rect
        Base::insert( items, iter.key(), iter.value() );
        boundingRect |= iter.value().boundingRect().translated( iter.value().position() );
    }

    // create pixmap
    QPixmap pixmap( boundingRect.size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::TextAntialiasing, true );
    painter.translate( -boundingRect.topLeft() );
    for( auto&& iter = items.constBegin(); iter != items.constEnd(); ++iter )
    {

        // setup option
        const auto index( model_->index( iter.key(), 0 ) );
        QStyleOptionViewItem option = _viewOptions( index );
        option.rect = iter.value().boundingRect();

        // paint
        painter.translate( iter.value().position() );
        iter.value().paint( &painter, &option, this );
        painter.translate( -iter.value().position() );

    }

    return pixmap;

}

//______________________________________________________________________
void IconView::_createFindDialog()
{

    Debug::Throw( "IconView::_createFindDialog.\n" );

    // create dialog
    findDialog_ =  new BaseFindDialog( this );
    findDialog_->setWindowTitle( tr( "Find in List" ) );

    if( !findWidget_ ) _createFindWidget( false );
    findDialog_->setBaseFindWidget( findWidget_ );

    return;

}

//______________________________________________________________________
void IconView::_createFindWidget( bool compact )
{

    Debug::Throw( "IconView::_createFindWidget.\n" );

    // create Widget
    auto findWidget = new BaseFindWidget( this, compact );
    findWidget->enableEntireWord( false );
    findWidget->enableRegExp( true );

    setFindWidget( findWidget );
}

//______________________________________________________________________
bool IconView::_findForward( const TextSelection& selection, bool rewind )
{
    Debug::Throw( "IconView::_findForward.\n" );
    if( selection.text().isEmpty() ) return false;

    // store selection
    TextEditor::setLastSelection( selection );

    // check model and selection model
    if( !( model_ && selectionModel() ) ) return false;

    QRegularExpression regexp;
    if( selection.flag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        regexp.setPattern( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.flag( TextSelection::CaseSensitive ) );

    }

    // set first index
    const auto current( selectionModel()->currentIndex() );
    auto index( ( selection.flag( TextSelection::NoIncrement ) ) ? current:_indexAfter( current ) );

    // if index index is invalid and rewind, set index index of the model
    if( (!index.isValid()) && rewind )
    {
        rewind = false;
        index = _firstIndex();
    }

    // no starting index found. Return
    if( !index.isValid() ) return false;

    // loop over indexes
    while( index.isValid() )
    {

        QString text;
        bool accepted( false );

        // check if column is visible
        if( !(text = model_->data( index ).toString() ).isEmpty() )
        {

            // check if text match
            if( regexp.isValid() && !regexp.pattern().isEmpty() ) { if( regexp.match( text ).hasMatch() ) accepted = true; }
            else if( text.indexOf( selection.text(), 0, selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) >= 0 )
            { accepted = true; }

        }

        if( accepted )
        {

            QItemSelectionModel::SelectionFlags command( QItemSelectionModel::Clear|QItemSelectionModel::Select );
            if( selectionBehavior() == SelectRows ) command |= QItemSelectionModel::Rows;
            selectionModel()->select( index, command );

            // update current index
            command = QItemSelectionModel::Current;
            if( selectionBehavior() == SelectRows ) command |= QItemSelectionModel::Rows;
            selectionModel()->setCurrentIndex( index,  command );

            // quit loop
            return true;

        } else {

            index = _indexAfter( index );
            if( rewind && !index.isValid() )
            {
                rewind = false;
                index = _firstIndex();

            }

        }

    }

    // no match found
    return false;

}

//______________________________________________________________________
bool IconView::_findBackward( const TextSelection& selection, bool rewind )
{

    Debug::Throw( "IconView::_findBackward.\n" );
    if( selection.text().isEmpty() ) return false;

    // store selection
    TextEditor::setLastSelection( selection );

    // check model and selection model
    if( !( model_ && selectionModel() ) ) return false;

    QRegularExpression regexp;
    if( selection.flag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        regexp.setPattern( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.flag( TextSelection::CaseSensitive ) );

    }

    // set first index
    const auto current( selectionModel()->currentIndex() );
    auto index( ( selection.flag( TextSelection::NoIncrement ) ) ? current:_indexBefore( current ) );

    // if index index is invalid and rewind, set index index of the model
    if( (!index.isValid()) && rewind ) {
        rewind = false;
        index = _lastIndex();
    }


    // no starting index found. Return
    if( !index.isValid() ) return false;

    // loop over indexes
    while( index.isValid() )
    {

        QString text;
        bool accepted( false );

        // check if column is visible
        if( !(text = model_->data( index ).toString() ).isEmpty() )
        {

            // check if text match
            if( regexp.isValid() && !regexp.pattern().isEmpty() ) { if( regexp.match( text ).hasMatch() ) accepted = true; }
            else if( text.indexOf( selection.text(), 0, selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) >= 0 )
            { accepted = true; }

        }

        if( accepted )
        {

            QItemSelectionModel::SelectionFlags command( QItemSelectionModel::Clear|QItemSelectionModel::Select );
            if( selectionBehavior() == SelectRows ) command |= QItemSelectionModel::Rows;
            selectionModel()->select( index, command );

            // update current index
            command = QItemSelectionModel::Current;
            if( selectionBehavior() == SelectRows ) command |= QItemSelectionModel::Rows;
            selectionModel()->setCurrentIndex( index,  command );

            // quit loop
            return true;

        } else {

            index = _indexBefore( index );
            if( rewind && !index.isValid() )
            {
                rewind = false;
                index = _lastIndex();
            }

        }

    }

    return false;

}

//____________________________________________________________________
QStyleOptionViewItem IconView::_viewOptions( const QModelIndex& index ) const
{

    // setup option
    QStyleOptionViewItem option = viewOptions();
    option.showDecorationSelected = true;

    // state
    if( selectionModel()->isSelected( index ) )
    {
        option.viewItemPosition = QStyleOptionViewItem::OnlyOne;
        option.state |= QStyle::State_Selected;
    }

    if( index == hoverIndex_ ) option.state |= QStyle::State_MouseOver;

    // color
    {
        QVariant variant( model_->data( index, Qt::ForegroundRole ) );
        if( variant.canConvert( QVariant::Color ) )
        { option.palette.setColor( QPalette::Text, variant.value<QColor>() ); }
    }

    // font
    {
        QVariant variant( model_->data( index, Qt::FontRole ) );
        if( variant.canConvert( QVariant::Font ) )
        { option.font = variant.value<QFont>(); }
    }

    return option;

}

//____________________________________________________________________
void IconView::updateGeometries()
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
    if( model_ ) model_->sort( column, order );
}

//_____________________________________________________________________
void IconView::_findFromDialog()
{
    Debug::Throw( "IconView::_findFromDialog.\n" );

    // set default text
    // update find text
    QString text( selection().text() );
    if( !text.isEmpty() )
    {
        const int maxLength( 1024 );
        text.truncate( maxLength );
    }

    // create
    if( useEmbeddedWidgets_ )
    {

        if( !findWidget_ ) _createFindWidget( true );
        findWidget_->show();

    } else {

        if( !findDialog_ ) _createFindDialog();
        findDialog_->centerOnParent();
        findDialog_->show();
        findDialog_->activateWindow();

    }

    findWidget_->synchronize();
    findWidget_->matchFound();
    findWidget_->setText( text );
    findWidget_->editor().setFocus();

    return;
}

//_____________________________________________________________________
void IconView::_updateHoverIndex()
{
    Debug::Throw( "IconView::_updateHoverIndex.\n" );
    _setHoverIndex(  indexAt( viewport()->mapFromGlobal( QCursor::pos() ) ) );
}

//_____________________________________________________________________
void IconView::_updateConfiguration()
{
    Debug::Throw( "IconView::_updateConfiguration.\n" );

    // update sort order from options
    updateSortOrder();

    // update pixmap size
    int iconSize( IconSize::Size::VeryLarge );
    if( iconSizeFromOptions_ && XmlOptions::get().contains( "ICON_VIEW_ICON_SIZE" ) && (iconSize = XmlOptions::get().get<int>( "ICON_VIEW_ICON_SIZE" )) != this->iconSize().width() )
    {
        QAbstractItemView::setIconSize( QSize( iconSize, iconSize ) );

        // update list of icon sizes
        _updateIconSizes( iconSize );

        if( model_ ) doItemsLayout();
    }
}

//________________________________________________
void IconView::_incrementIconSize( int delta )
{

    // get current iconsize
    auto index = iconSizes_.indexOf( iconSize().width() );
    if( index < 0 ) return;

    // update icon size
    const auto newSize( iconSizes_[qBound( 0, index+delta, iconSizes_.size()-1 )] );
    setIconSize( QSize( newSize, newSize ) );

}

//__________________________________________________________
void IconView::_installActions()
{
    Debug::Throw( "IconView::_installActions.\n" );

    addAction( selectAllAction_ = new QAction( tr( "Select All" ), this ) );
    selectAllAction_->setShortcut( QKeySequence::SelectAll );
    selectAllAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( selectAllAction_, &QAction::triggered, this, &QAbstractItemView::selectAll );

    addAction( findAction_ = new QAction( IconEngine::get( IconNames::Find ), tr( "Find" ), this ) );
    findAction_->setShortcut( QKeySequence::Find );
    findAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAction_, &QAction::triggered, this, &IconView::_findFromDialog );

    addAction( findAgainAction_ = new QAction( tr( "Find Again" ), this ) );
    findAgainAction_->setShortcut( Qt::CTRL + Qt::Key_G );
    findAgainAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainAction_, &QAction::triggered, this, &IconView::findAgainForward );

    addAction( findAgainBackwardAction_ = new QAction( this ) );
    findAgainBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_G );
    findAgainBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainBackwardAction_, &QAction::triggered, this, &IconView::findAgainBackward );

    addAction( findSelectionAction_ = new QAction( tr( "Find Selection" ), this ) );
    findSelectionAction_->setShortcut( Qt::CTRL + Qt::Key_H );
    findSelectionAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionAction_, &QAction::triggered, this, &IconView::findSelectionForward );

    addAction( findSelectionBackwardAction_ = new QAction( this ) );
    findSelectionBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_H );
    findSelectionBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionBackwardAction_, &QAction::triggered, this, &IconView::findSelectionBackward );

    addAction( incrementIconSizeAction_ = new QAction( tr( "Increase Icon Size" ), this ) );
    incrementIconSizeAction_->setShortcut( QKeySequence::ZoomIn );
    incrementIconSizeAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( incrementIconSizeAction_, &QAction::triggered, this, &IconView::_incrementIconSize );

    addAction( decrementIconSizeAction_ = new QAction( tr( "Decrease Icon Size" ), this ) );
    decrementIconSizeAction_->setShortcut( QKeySequence::ZoomOut );
    decrementIconSizeAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( decrementIconSizeAction_, &QAction::triggered, this, &IconView::_decrementIconSize );
}


//_________________________________________________________
QModelIndex IconView::_firstIndex() const
{ return items_.empty() ? QModelIndex():model_->index( 0, 0 ); }

//_________________________________________________________
QModelIndex IconView::_lastIndex() const
{ return items_.empty() ? QModelIndex():model_->index( 0, items_.size()-1 ); }

//_________________________________________________________
QModelIndex IconView::_indexAfter( const QModelIndex& current ) const
{ return (current.row() >= items_.size()) ? QModelIndex():model_->index( current.row()+1, 0 ); }

//_________________________________________________________
QModelIndex IconView::_indexBefore( const QModelIndex& current ) const
{ return (items_.empty() || current.row() == 0 || current.row() > items_.size() ) ? QModelIndex():model_->index( current.row()-1, 0 ); }

//_________________________________________________________
IconView::Container::Container( QWidget* parent ):
    QWidget( parent ),
    Counter( "IconView::Container" ),
    iconView_( new IconView )
{ _initialize(); }

//_________________________________________________________
IconView::Container::Container( QWidget* parent, IconView* iconView ):
    QWidget( parent ),
    Counter( "IconView::Container" ),
    iconView_( iconView )
{ _initialize(); }

//_________________________________________________________
void IconView::Container::_initialize()
{
    Debug::Throw( "IconView::Container::_initialize.\n" );
    iconView_->setParent( this );

    // setup layout
    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(2);
    setLayout( vLayout );

    // iconView
    vLayout->addWidget( iconView_ );
    iconView_->useEmbeddedWidgets_ = true;

    // find widget
    iconView_->_createFindWidget( true );
    iconView_->findWidget_->setParent( this );
    vLayout->addWidget( iconView_->findWidget_ );
    iconView_->findWidget_->hide();

    connect( &iconView_->findWidget_->closeButton(), &QPushButton::clicked, [this](bool){ iconView_->setFocus(); } );

}

//___________________________________________________
void IconView::_updateIconSizes( int iconSize )
{
    Debug::Throw( "IconView::_updateIconSizes.\n" );

    // update available iconSizes
    auto iter = std::lower_bound( iconSizes_.begin(), iconSizes_.end(), iconSize );
    if( iter == iconSizes_.end() || *iter != iconSize )
    { iconSizes_.insert( iter, iconSize ); }

}
