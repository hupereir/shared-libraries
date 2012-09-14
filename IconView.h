#ifndef IconView_h
#define IconView_h

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
*******************************************************************************/

#include "Counter.h"
#include "IconViewItem.h"
#include "TextSelection.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QString>
#include <QtCore/QTimerEvent>

#include <QtGui/QAbstractItemView>
#include <QtGui/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QRubberBand>
#include <QtGui/QScrollBar>

// forward declaration
class BaseFindDialog;

//! icon view
class IconView: public QAbstractItemView, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    IconView( QWidget* = 0 );

    //! destructor
    virtual ~IconView( void )
    {}

    //! enable list finding
    void setFindEnabled( bool value );

    //! TextSelection object from this selection, or clipboard
    TextSelection selection( void ) const;

    //! header
    QHeaderView* header( void ) const
    { return header_; }

    //! sorting
    void setSortingEnabled( bool value )
    { header_->setSortIndicatorShown( value ); }

    //! set model
    virtual void 	setModel( QAbstractItemModel* model )
    {
        QAbstractItemView::setModel( model );
        header_->setModel( model );
    }

    //! set spacing
    void setMargin( int value )
    {
        if( margin_ == value ) return;
        margin_ = value;
    }

    //! set spacing
    void setSpacing( int value )
    {
        if( spacing_ == value ) return;
        spacing_ = value;
    }

    //!@name stored options
    //@{

    //! option name
    virtual bool setOptionName( const QString& );

    //! option name
    virtual bool hasOptionName( void ) const
    { return !sortColumnOptionName().isEmpty(); }

    //! sort column option name
    virtual const QString& sortColumnOptionName( void ) const
    { return sortColumnOptionName_; }

    //! sort order option name
    virtual const QString& sortOrderOptionName( void ) const
    { return sortOrderOptionName_; }

    //@}

    //!@name actions
    //@{

    //! select all
    QAction& selectAllAction( void ) const
    { return *selectAllAction_; }

    //! find from dialog
    QAction& findAction( void ) const
    { return *findAction_; }

    //! find selection again
    QAction& findSelectionAction( void ) const
    { return *findSelectionAction_; }

    //! find again
    QAction& findAgainAction( void ) const
    { return *findAgainAction_; }

    //@}

    //! return index at a given position
    virtual QModelIndex indexAt( const QPoint& ) const;

    //! scroll to given index
    virtual void scrollTo( const QModelIndex&, ScrollHint );

    //! visual rect for given index
    virtual QRect visualRect( const QModelIndex& ) const;

    //! minimum size hind
    virtual QSize minimumSizeHint( void ) const;

    signals:

    //! emmited when item is hovered. Invalid index means no hovered index
    void hovered( const QModelIndex& );

    //! emmitted when selection could not be found
    void noMatchFound( void );

    //! emmitted when selection could be found
    void matchFound( void );

    public slots:

    //! sort order
    virtual void updateSortOrder( void );

    //! sort order
    virtual void saveSortOrder( void );

    // items layout
    virtual void doItemsLayout();

    //! find next occurence of TextSelection
    virtual void find( TextSelection selection );

    //! find current selection forward
    virtual void findSelectionForward( void )
    { _findForward( selection(), true ); }

    //! find current selection backward
    virtual void findSelectionBackward( void )
    { _findBackward( selection(), true ); }

    //! find last search forward
    virtual void findAgainForward( void );

    //! find last search forward
    virtual void findAgainBackward( void );

    protected:

    // true if index is hidden
    virtual bool isIndexHidden( const QModelIndex& ) const;

    //! move curesor
    virtual QModelIndex moveCursor( CursorAction, Qt::KeyboardModifiers );

    //! selection
    virtual void setSelection( const QRect&, QItemSelectionModel::SelectionFlags );

    //! start drag
    virtual void startDrag( Qt::DropActions );

    //! contents scroll
    virtual void scrollContentsBy( int, int );

    //! horizontal offset
    virtual int horizontalOffset( void ) const;

    //! vertical offset
    virtual int verticalOffset( void ) const;

    //! region for given selection
    virtual QRegion visualRegionForSelection( const QItemSelection& ) const;

    //! event
    virtual bool event( QEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! resize event
    virtual void resizeEvent( QResizeEvent* );

    //! keypress event
    virtual void keyPressEvent( QKeyEvent* );

    //! mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //! mouse move
    virtual void mouseMoveEvent( QMouseEvent* );

    //! mouse release
    virtual void mouseReleaseEvent( QMouseEvent* );

    //! drag enter
    virtual void dragEnterEvent( QDragEnterEvent* );

    //! drag move
    virtual void dragMoveEvent( QDragMoveEvent* );

    //! drag leave
    virtual void dragLeaveEvent( QDragLeaveEvent* );

    //! drag enter
    virtual void dropEvent( QDropEvent* );

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    //! selection
    QModelIndexList _selectedIndexes( const QRect& ) const;

    //! hover index
    virtual const QModelIndex& _hoverIndex( void ) const
    { return hoverIndex_; }

    //! hover index
    virtual void _setHoverIndex( const QModelIndex& );

    //! update item from index
    void _updateItem( IconViewItem& item, const QModelIndex& index ) const;

    //! layout existing items
    void _layoutItems( void );

    //! scrollbar position
    QPoint _scrollBarPosition( void ) const
    {
        return QPoint(
            horizontalScrollBar()->isVisible() ? horizontalScrollBar()->value():0,
            verticalScrollBar()->isVisible() ? verticalScrollBar()->value():0 );
    }

    //! get pixmap for a given index selection
    QPixmap _pixmap( const QModelIndexList&, QRect& );

    //! find dialog
    virtual BaseFindDialog& _findDialog( void )
    { return *findDialog_; }

    //! find dialog
    virtual void _createBaseFindDialog( void );

    //! find selection in forward direction
    virtual bool _findForward( const TextSelection& selection, bool rewind );

    //! find selection in backward direction
    virtual bool _findBackward( const TextSelection& selection, bool rewind );

    protected slots:

    //! update geometries
    virtual void updateGeometries( void );

    //! sort order
    void sortByColumn( int, Qt::SortOrder );

    //! find text from dialog
    virtual void _findFromDialog( void );

    //! update hover index
    virtual void _updateHoverIndex( void );

    private slots:

    //! update alternate item color
    void _updateConfiguration( void );

    private:

    //! install actions
    virtual void _installActions( void );

    //! return first index
    QModelIndex _firstIndex() const;

    //! return last index
    QModelIndex _lastIndex() const;

    //! return next index of current
    QModelIndex _indexAfter( const QModelIndex& ) const;

    //! return previous index
    QModelIndex _indexBefore( const QModelIndex& ) const;

    //! headerView
    QHeaderView* header_;

    //! items
    IconViewItem::Map items_;

    //! find dialog
    BaseFindDialog* findDialog_;

    //!@name actions
    //@{

    //! select all items
    QAction* selectAllAction_;

    //! find from dialog
    QAction* findAction_;

    //! find selection again
    QAction* findSelectionAction_;

    //! find selection backward
    QAction* findSelectionBackwardAction_;

    //! find again
    QAction* findAgainAction_;

    //! find again backward
    QAction* findAgainBackwardAction_;

    //@}

    //! icon size
    QSize pixmapSize_;

    //! margin
    int margin_;

    //! spacing
    int spacing_;

    //! sort column option name
    QString sortColumnOptionName_;

    //! sort order option name
    QString sortOrderOptionName_;

    //! column count
    int columnCount_;

    //! row count
    int rowCount_;

    //! total rect
    QRect boundingRect_;

    //! rubber band
    QRubberBand* rubberBand_;

    //! drag button
    Qt::MouseButton dragButton_;

    //! drag origin
    QPoint dragOrigin_;

    //! true if drag is in progress
    bool dragInProgress_;

    //! autoscroll timer
    QBasicTimer autoScrollTimer_;

    //! anchor index (for mouse selection)
    QModelIndex anchorIndex_;

    //! hovered index
    QModelIndex hoverIndex_;

};

#endif
