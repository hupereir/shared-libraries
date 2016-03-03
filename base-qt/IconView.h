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
class IconView: public QAbstractItemView, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    IconView( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* TextSelection object from this selection, or clipboard
    TextSelection selection( void ) const;

    //* header
    QHeaderView* header( void ) const
    { return header_; }

    //* option name
    virtual bool hasOptionName( void ) const
    { return !sortColumnOptionName().isEmpty(); }

    //* sort column option name
    virtual const QString& sortColumnOptionName( void ) const
    { return sortColumnOptionName_; }

    //* sort order option name
    virtual const QString& sortOrderOptionName( void ) const
    { return sortOrderOptionName_; }

    //* return index at a given position
    virtual QModelIndex indexAt( const QPoint& ) const;

    //* visual rect for given index
    virtual QRect visualRect( const QModelIndex& ) const;

    //* minimum size hind
    virtual QSize minimumSizeHint( void ) const;

    //@}

    //*@name modifiers
    //@{

    //* set model
    virtual void setModel( QAbstractItemModel* );

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
    virtual bool setOptionName( const QString& );

    //* scroll to given index
    virtual void scrollTo( const QModelIndex&, ScrollHint );

    //@}

    //*@name actions
    //@{

    //* select all
    QAction& selectAllAction( void ) const
    { return *selectAllAction_; }

    //* find from dialog
    QAction& findAction( void ) const
    { return *findAction_; }

    //* find selection again
    QAction& findSelectionAction( void ) const
    { return *findSelectionAction_; }

    //* find again
    QAction& findAgainAction( void ) const
    { return *findAgainAction_; }

    //@}

    //* container class for embedded Find dialog
    class Container: public QWidget, public Counter
    {

        public:

        //* constructor
        Container( QWidget* = nullptr );

        //* constructor
        Container( QWidget*, IconView* );

        //*@name accessors
        IconView& iconView( void ) const
        { return *iconView_; }

        private:

        //* initialize
        void _initialize( void );

        //* contained iconView
        IconView* iconView_;

    };

    Q_SIGNALS:

    //* emitted when item is hovered. Invalid index means no hovered index
    void hovered( const QModelIndex& );

    //* emitted when selection could not be found
    void noMatchFound( void );

    //* emitted when selection could be found
    void matchFound( void );

    #if QT_VERSION < 0x050000
    //* icon size changed signal
    void iconSizeChanged( const QSize& );
    #endif

    public Q_SLOTS:

    //* sort order
    virtual void updateSortOrder( void );

    //* sort order
    virtual void saveSortOrder( void );

    // items layout
    virtual void doItemsLayout();

    //* find next occurence of TextSelection
    virtual void find( TextSelection selection );

    //* find current selection forward
    virtual void findSelectionForward( void )
    { _findForward( selection(), true ); }

    //* find current selection backward
    virtual void findSelectionBackward( void )
    { _findBackward( selection(), true ); }

    //* find last search forward
    virtual void findAgainForward( void );

    //* find last search forward
    virtual void findAgainBackward( void );

    //* store selected indexes in model
    void storeSelectedIndexes( void );

    //* restore selected indexes from model
    void restoreSelectedIndexes( void );

    protected:

    // true if index is hidden
    virtual bool isIndexHidden( const QModelIndex& ) const;

    //* move curesor
    virtual QModelIndex moveCursor( CursorAction, Qt::KeyboardModifiers );

    //* selection
    virtual void setSelection( const QRect&, QItemSelectionModel::SelectionFlags );

    //* start drag
    virtual void startDrag( Qt::DropActions );

    //* contents scroll
    virtual void scrollContentsBy( int, int );

    //* horizontal offset
    virtual int horizontalOffset( void ) const;

    //* vertical offset
    virtual int verticalOffset( void ) const;

    //* region for given selection
    virtual QRegion visualRegionForSelection( const QItemSelection& ) const;

    //* event
    virtual bool event( QEvent* );

    //* paint event
    virtual void paintEvent( QPaintEvent* );

    //* resize event
    virtual void resizeEvent( QResizeEvent* );

    //* keypress event
    virtual void keyPressEvent( QKeyEvent* );

    //* mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //* mouse move
    virtual void mouseMoveEvent( QMouseEvent* );

    //* mouse release
    virtual void mouseReleaseEvent( QMouseEvent* );

    //* drag enter
    virtual void dragEnterEvent( QDragEnterEvent* );

    //* drag move
    virtual void dragMoveEvent( QDragMoveEvent* );

    //* drag leave
    virtual void dragLeaveEvent( QDragLeaveEvent* );

    //* drag enter
    virtual void dropEvent( QDropEvent* );

    //* timer event
    virtual void timerEvent( QTimerEvent* );

    //* selection
    QModelIndexList _selectedIndexes( const QRect& ) const;

    //* hover index
    virtual const QModelIndex& _hoverIndex( void ) const
    { return hoverIndex_; }

    //* hover index
    virtual void _setHoverIndex( const QModelIndex& );

    //* update item from index
    virtual void _updateItem( IconViewItem&, const QModelIndex& ) const;

    //* layout existing items
    void _layoutItems( void );

    //* scrollbar position
    QPoint _scrollBarPosition( void ) const
    {
        return QPoint(
            horizontalScrollBar()->isVisible() ? horizontalScrollBar()->value():0,
            verticalScrollBar()->isVisible() ? verticalScrollBar()->value():0 );
    }

    //* get pixmap for a given index selection
    QPixmap _pixmap( const QModelIndexList&, QRect& );

    //* find dialog
    virtual void _createFindDialog( void );

    //* find widget
    virtual void _createFindWidget( bool compact );

    //* find selection in forward direction
    virtual bool _findForward( const TextSelection& selection, bool rewind );

    //* find selection in backward direction
    virtual bool _findBackward( const TextSelection& selection, bool rewind );

    //* get view options matching a given index
    virtual QStyleOptionViewItemV4 _viewOptions( const QModelIndex& ) const;

    protected Q_SLOTS:

    //* update geometries
    virtual void updateGeometries( void );

    //* sort order
    void sortByColumn( int, Qt::SortOrder );

    //* find text from dialog
    virtual void _findFromDialog( void );

    //* update hover index
    virtual void _updateHoverIndex( void );

    private Q_SLOTS:

    //* update alternate item color
    void _updateConfiguration( void );

    private:

    //* install actions
    virtual void _installActions( void );

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
