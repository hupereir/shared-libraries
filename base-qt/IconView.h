#ifndef IconView_h
#define IconView_h

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

#include "Counter.h"
#include "IconViewItem.h"
#include "TextSelection.h"

#include <QBasicTimer>
#include <QString>
#include <QTimerEvent>

#include <QAbstractItemView>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QRubberBand>
#include <QScrollBar>
#include <QStyleOptionViewItemV4>

// forward declaration
class BaseFindDialog;
class BaseFindWidget;
class ItemModel;

//* icon view
class IconView: public QAbstractItemView, private Base::Counter<IconView>
{

    Q_OBJECT

    public:

    //* constructor
    explicit IconView( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* TextSelection object from this selection, or clipboard
    TextSelection selection() const;

    //* header
    QHeaderView* header() const
    { return header_; }

    //* option name
    bool hasOptionName() const
    { return !sortColumnOptionName().isEmpty(); }

    //* sort column option name
    const QString& sortColumnOptionName() const
    { return sortColumnOptionName_; }

    //* sort order option name
    const QString& sortOrderOptionName() const
    { return sortOrderOptionName_; }

    //* return index at a given position
    QModelIndex indexAt( const QPoint& ) const override;

    //* visual rect for given index
    QRect visualRect( const QModelIndex& ) const override;

    //* minimum size hind
    QSize minimumSizeHint() const override;

    //@}

    //*@name modifiers
    //@{

    //* set model
    void setModel( QAbstractItemModel* ) override;

    //* enable list finding
    void setFindEnabled( bool value );

    //* sorting
    void setSortingEnabled( bool value )
    { header_->setSortIndicatorShown( value ); }

    //* set spacing
    void setMargin( int value )
    {
        if( margin_ == value ) return;
        margin_ = value;
    }

    //* set spacing
    void setSpacing( int value )
    {
        if( spacing_ == value ) return;
        spacing_ = value;
    }

    //* set icon size manually and disable option
    void setIconSize( const QSize& size )
    {

        if( size != iconSize() )
        {
            QAbstractItemView::setIconSize( size );

            #if QT_VERSION < 0x050000
            emit iconSizeChanged( size );
            #endif

        }

        iconSizeFromOptions_ = false;

    }

    //* option name
    bool setOptionName( const QString& );

    //* scroll to given index
    void scrollTo( const QModelIndex&, ScrollHint ) override;

    //@}

    //*@name actions
    //@{

    //* select all
    QAction& selectAllAction() const
    { return *selectAllAction_; }

    //* find from dialog
    QAction& findAction() const
    { return *findAction_; }

    //* find selection again
    QAction& findSelectionAction() const
    { return *findSelectionAction_; }

    //* find again
    QAction& findAgainAction() const
    { return *findAgainAction_; }

    //@}

    //* container class for embedded Find dialog
    class Container: public QWidget, private Base::Counter<Container>
    {

        public:

        //* constructor
        explicit Container( QWidget* = nullptr );

        //* constructor
        explicit Container( QWidget*, IconView* );

        //*@name accessors
        IconView& iconView() const
        { return *iconView_; }

        private:

        //* initialize
        void _initialize();

        //* contained iconView
        IconView* iconView_;

    };

    Q_SIGNALS:

    //* emitted when item is hovered. Invalid index means no hovered index
    void hovered( const QModelIndex& );

    //* emitted when selection could not be found
    void noMatchFound();

    //* emitted when selection could be found
    void matchFound();

    #if QT_VERSION < 0x050000
    //* icon size changed signal
    void iconSizeChanged( const QSize& );
    #endif

    public Q_SLOTS:

    //* sort order
    void updateSortOrder();

    //* sort order
    void saveSortOrder();

    // items layout
    void doItemsLayout();

    //* find next occurence of TextSelection
    void find( TextSelection selection );

    //* find current selection forward
    void findSelectionForward()
    { _findForward( selection(), true ); }

    //* find current selection backward
    void findSelectionBackward()
    { _findBackward( selection(), true ); }

    //* find last search forward
    void findAgainForward();

    //* find last search forward
    void findAgainBackward();

    //* store selected indexes in model
    void storeSelectedIndexes();

    //* restore selected indexes from model
    void restoreSelectedIndexes();

    protected:

    //* true if index is hidden
    bool isIndexHidden( const QModelIndex& ) const override;

    //* move curesor
    QModelIndex moveCursor( CursorAction, Qt::KeyboardModifiers ) override;

    //* selection
    void setSelection( const QRect&, QItemSelectionModel::SelectionFlags ) override;

    //* start drag
    void startDrag( Qt::DropActions ) override;

    //* contents scroll
    void scrollContentsBy( int, int ) override;

    //* horizontal offset
    int horizontalOffset() const override;

    //* vertical offset
    int verticalOffset() const override;

    //* region for given selection
    QRegion visualRegionForSelection( const QItemSelection& ) const override;

    //* event
    bool event( QEvent* ) override;

    //* paint event
    void paintEvent( QPaintEvent* ) override;

    //* resize event
    void resizeEvent( QResizeEvent* ) override;

    //* keypress event
    void keyPressEvent( QKeyEvent* ) override;

    //* mouse press
    void mousePressEvent( QMouseEvent* ) override;

    //* mouse move
    void mouseMoveEvent( QMouseEvent* ) override;

    //* mouse release
    void mouseReleaseEvent( QMouseEvent* ) override;

    //* drag enter
    void dragEnterEvent( QDragEnterEvent* ) override;

    //* drag move
    void dragMoveEvent( QDragMoveEvent* ) override;

    //* drag leave
    void dragLeaveEvent( QDragLeaveEvent* ) override;

    //* drag enter
    void dropEvent( QDropEvent* ) override;

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    //* selection
    QModelIndexList _selectedIndexes( const QRect& ) const;

    //* hover index
    const QModelIndex& _hoverIndex() const
    { return hoverIndex_; }

    //* hover index
    void _setHoverIndex( const QModelIndex& );

    //* update item from index
    virtual void _updateItem( IconViewItem&, const QModelIndex& ) const;

    //* layout existing items
    void _layoutItems();

    //* scrollbar position
    QPoint _scrollBarPosition() const
    {
        return QPoint(
            horizontalScrollBar()->isVisible() ? horizontalScrollBar()->value():0,
            verticalScrollBar()->isVisible() ? verticalScrollBar()->value():0 );
    }

    //* get pixmap for a given index selection
    QPixmap _pixmap( const QModelIndexList&, QRect& );

    //* find dialog
    void _createFindDialog();

    //* find widget
    void _createFindWidget( bool compact );

    //* find selection in forward direction
    bool _findForward( const TextSelection& selection, bool rewind );

    //* find selection in backward direction
    bool _findBackward( const TextSelection& selection, bool rewind );

    //* get view options matching a given index
    QStyleOptionViewItemV4 _viewOptions( const QModelIndex& ) const;

    protected Q_SLOTS:

    //* update geometries
    void updateGeometries();

    //* sort order
    void sortByColumn( int, Qt::SortOrder );

    //* find text from dialog
    void _findFromDialog();

    //* update hover index
    void _updateHoverIndex();

    private Q_SLOTS:

    //* update alternate item color
    void _updateConfiguration();

    private:

    //* install actions
    void _installActions();

    //* return first index
    QModelIndex _firstIndex() const;

    //* return last index
    QModelIndex _lastIndex() const;

    //* return next index of current
    QModelIndex _indexAfter( const QModelIndex& ) const;

    //* return previous index
    QModelIndex _indexBefore( const QModelIndex& ) const;

    //* headerView
    QHeaderView* header_ = nullptr;

    //* items
    IconViewItem::Map items_;

    //* find dialog
    BaseFindDialog* findDialog_ = nullptr;

    //* find widget
    BaseFindWidget* findWidget_ = nullptr;

    //* model
    ItemModel* model_ = nullptr;

    //*@name actions
    //@{

    //* select all items
    QAction* selectAllAction_ = nullptr;

    //* find from dialog
    QAction* findAction_ = nullptr;

    //* find selection again
    QAction* findSelectionAction_ = nullptr;

    //* find selection backward
    QAction* findSelectionBackwardAction_ = nullptr;

    //* find again
    QAction* findAgainAction_ = nullptr;

    //* find again backward
    QAction* findAgainBackwardAction_ = nullptr;

    //@}

    //* true if icon size is to be set from options
    bool iconSizeFromOptions_ = true;

    //* margin
    int margin_ = 15;

    //* spacing
    int spacing_ = 15;

    //* sort column option name
    QString sortColumnOptionName_;

    //* sort order option name
    QString sortOrderOptionName_;

    //* column count
    int columnCount_ = 1;

    //* row count
    int rowCount_ = 1;

    //* true if use dialog for finding
    bool useEmbeddedWidgets_ = false;

    //* total rect
    QRect boundingRect_;

    //* rubber band
    QRubberBand* rubberBand_ = nullptr;

    //* drag button
    Qt::MouseButton dragButton_ = Qt::NoButton;

    //* drag origin
    QPoint dragOrigin_;

    //* true if drag is in progress
    bool dragInProgress_ = false;

    //* autoscroll timer
    QBasicTimer autoScrollTimer_;

    //* anchor index (for mouse selection)
    QModelIndex anchorIndex_;

    //* hovered index
    QModelIndex hoverIndex_;

    friend class Container;

};

#endif
