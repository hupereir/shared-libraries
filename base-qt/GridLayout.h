#ifndef GridLayout_h
#define GridLayout_h

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
#include "base_qt_export.h"

#include <QGridLayout>
#include <QList>

class BASE_QT_EXPORT GridLayout: public QGridLayout, private Base::Counter<GridLayout>
{

    Q_OBJECT

    public:

    //* constructor
    explicit GridLayout():
        QGridLayout(),
        Counter( QStringLiteral("GridLayout") )
    {}

    //* column alignments
    using AlignmentList = QList<Qt::Alignment>;

    //*@name accessors
    //@{

    //* current row
    int currentRow() const
    { return row_; }

    //* current column
    int currentColumn() const
    { return column_; }

    //@}

    //*@names modifiers
    //@{

    //* set columns
    void setMaxCount( int maxCount )
    {
        maxCount_ = maxCount;
        columnAlignments_.clear();

        // initialize columnAlignments
        for( int index = 0; index < maxCount; ++index )
        { columnAlignments_.append(Qt::Alignment()); }

    }

    //* set column alignment
    void setColumnAlignment( int column, const Qt::Alignment alignment )
    { columnAlignments_[column] = alignment; }

    //* set orientation
    void setOrientation( Qt::Orientation orientation )
    { orientation_ = orientation; }

    //* add widget
    void addWidget( QWidget* widget, int row, int column, Qt::Alignment alignment = {} )
    { GridLayout::addWidget( widget, row, column, 1, 1, alignment ); }

    //* add widget
    void addWidget ( QWidget * widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = {} )
    {
        Q_ASSERT( rowSpan > 0 );
        Q_ASSERT( columnSpan > 0 );
        if( alignment == 0 && _boundCheck( column ) && columnSpan == 1 ) alignment = columnAlignments_[column];
        QGridLayout::addWidget( widget, row, column, rowSpan, columnSpan, alignment );
        setLocation( row+rowSpan-1, column+columnSpan-1 );
        increment();
    }

    //* add widget
    void addWidget( QWidget* widget, Qt::Alignment alignment = {} )
    {
        Q_ASSERT( maxCount_ > 0 );
        if( alignment == 0 &&  _boundCheck( column_ ) ) alignment = columnAlignments_[column_];
        QGridLayout::addWidget( widget, row_, column_, alignment );
        increment();
    }

    //* add layout
    void addLayout( QLayout* layout, int row, int column, Qt::Alignment alignment = {} )
    { GridLayout::addLayout( layout, row, column, 1, 1, alignment ); }

    //* add layout
    void addLayout( QLayout* layout, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = {} )
    {
        Q_ASSERT( rowSpan > 0 );
        Q_ASSERT( columnSpan > 0 );
        if( alignment == 0 && _boundCheck( column ) && columnSpan == 1 ) alignment = columnAlignments_[column];
        QGridLayout::addLayout( layout, row, column, rowSpan, columnSpan, alignment );
        setLocation( row+rowSpan-1, column+columnSpan-1 );
        increment();
    }

    //* add layout
    void addLayout( QLayout* layout, Qt::Alignment alignment = {} )
    {
        Q_ASSERT( maxCount_ > 0 );
        if( alignment == 0 &&  _boundCheck( column_ ) ) alignment = columnAlignments_[column_];
        QGridLayout::addLayout( layout, row_, column_, alignment );
        increment();
    }

    using QGridLayout::addItem;

    //* add LayoutItem
    void addItem( QLayoutItem* item, int row, int column, Qt::Alignment alignment = {} )
    { GridLayout::addItem( item, row, column, 1, 1, alignment ); }

    //* add LayoutItem
    void addItem( QLayoutItem* item, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = {} )
    {
        Q_ASSERT( rowSpan > 0 );
        Q_ASSERT( columnSpan > 0 );
        if( alignment == 0 && _boundCheck( column ) && columnSpan == 1 ) alignment = columnAlignments_[column];
        QGridLayout::addItem( item, row, column, rowSpan, columnSpan, alignment );
        setLocation( row+rowSpan-1, column+columnSpan-1 );
        increment();
    }

    //* add LayoutItem
    void addItem( QLayoutItem* item, Qt::Alignment alignment = {} )
    {
        Q_ASSERT( maxCount_ > 0 );
        if( alignment == 0 &&  _boundCheck( column_ ) ) alignment = columnAlignments_[column_];
        QGridLayout::addItem( item, row_, column_, alignment );
        increment();
    }


    //* increment from last position
    void increment()
    {

        if( orientation_ == Qt::Vertical )
        {
            column_ ++;
            if( column_ >= maxCount_ )
            {
                column_ = 0;
                row_ ++;
            }
        } else {
            row_++;
            if( row_ >= maxCount_ )
            {
                row_ = 0;
                column_++;
            }
        }
    }

    //* set current position in grid
    void setLocation( int row, int column )
    { row_ = row; column_ = column; }

    //@}

    private:

    //* bound check
    bool _boundCheck( int column ) const
    { return column >= 0 && column < int( columnAlignments_.size() ); }

    //* orientation
    Qt::Orientation orientation_ = Qt::Vertical;

    //* number of columns
    int maxCount_ = 0;

    //* column alignments
    AlignmentList columnAlignments_;

    //* current column
    int column_ = 0;

    //* current row
    int row_ = 0;

};

#endif
