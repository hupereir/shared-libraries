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
*
*******************************************************************************/

#include "Counter.h"

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
#include <QtGui/QStyleOption>

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

    //! return index at a given position
    virtual QModelIndex indexAt( const QPoint& ) const;

    //! scroll to given index
    virtual void scrollTo( const QModelIndex&, ScrollHint );

    //! visual rect for given index
    virtual QRect visualRect( const QModelIndex& ) const;

    signals:

    void hovered( const QModelIndex& );

    public slots:

    //! sort order
    virtual void updateSortOrder( void );

    //! sort order
    virtual void saveSortOrder( void );

    // items layout
    virtual void doItemsLayout();

    protected slots:

    //! update geometries
    virtual void updateGeometries( void );

    //! sort order
    void sortByColumn( int, Qt::SortOrder );

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

    //! drag move
    virtual void dragMoveEvent( QDragMoveEvent* );

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    //! selection
    QModelIndexList _selectedIndexes( const QRect& ) const;

    //! hover index
    virtual const QModelIndex& _hoverIndex( void ) const
    { return hoverIndex_; }

    //! hover index
    virtual void _setHoverIndex( const QModelIndex& index )
    {
        if( hoverIndex_ == index ) return;
        hoverIndex_ = index;
        emit hovered( index );
    }

    //! graphics item
    class Item: public Counter
    {

        public:

        //! constructor
        Item( void ):
            Counter( "IconView::Item" ),
            dirty_( true ),
            margin_( 5 ),
            spacing_( 5 ),
            maxTextWidth_( 100 ),
            row_( -1 ),
            column_( -1 )
        {}

        //! destructor
        virtual ~Item( void )
        {}

        //!@name accessors
        //@{

        //! icon
        const QPixmap& pixmap( void ) const
        { return pixmap_; }

        //! text
        const QString& text( void ) const
        { return text_; }

        //! position
        QPoint position( void ) const
        { return position_; }

        //! row
        int row( void ) const
        { return row_; }

        //! column
        int column( void ) const
        { return column_; }

        //! bounding rect
        virtual QRect boundingRect( void ) const;

        //@}

        //!@name modifiers
        //@{

        //! set spacing
        void setMargin( int value )
        {
            if( margin_ == value ) return;
            margin_ = value;
            dirty_ = true;
        }

        //! set spacing
        void setSpacing( int value )
        {
            if( spacing_ == value ) return;
            spacing_ = value;
            dirty_ = true;
        }

        //! set icon
        void setPixmap( const QPixmap& pixmap )
        {
            const bool changed( pixmap_.size() != pixmap.size() );
            pixmap_ = pixmap;
            if( changed ) dirty_ = true;
        }

        //! set text
        void setText( const QString& text )
        {
            if( text_ == text ) return;
            text_ = text;
            dirty_ = true;
        }

        //! set position
        void setPosition( const QPoint& position )
        { position_ = position; }

        //! set location
        void setLocation( int row, int column )
        {
            row_ = row;
            column_ = column;
        }

        //@}

        //! item map
        typedef QMap<int, Item> Map;

        //! item list
        typedef QList<Item> List;

        //! paint
        virtual void paint( QPainter*, const QStyleOption*, QWidget* ) const;

        protected:

        //! update bounding rect
        void _updateBoundingRect( void );

        private:

        //! dirty
        bool dirty_;

        //! margin
        int margin_;

        //! spacing
        int spacing_;

        //! max text width
        int maxTextWidth_;

        //! pixmap
        QPixmap pixmap_;

        //! text
        QString text_;

        //! bounding rect
        QRect boundingRect_;

        //! position
        QPoint position_;

        //! row and column index
        int row_;

        //! column
        int column_;

    };

    //! update item from index
    void _updateItem( Item& item, const QModelIndex& index ) const;

    //! layout existing items
    void _layoutItems( void );

    //! scrollbar position
    QPoint _scrollBarPosition( void ) const
    { return QPoint( horizontalScrollBar()->value(), verticalScrollBar()->value() ); }

    //! get pixmap for a given index selection
    QPixmap _pixmap( const QModelIndexList&, QRect& );

    private slots:

    //! update alternate item color
    void _updateConfiguration( void );

    private:

    //! headerView
    QHeaderView* header_;

    //! items
    Item::Map items_;

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
