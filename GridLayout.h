#ifndef GridLayout_h
#define GridLayout_h
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

#include <QtGui/QGridLayout>
#include <QtCore/QVector>
#include <cassert>

class GridLayout: public QGridLayout, public Counter
{

    public:

    //! constructor
    GridLayout( void ):
        QGridLayout(),
        Counter( "GridLayout" ),
        orientation_( Qt::Vertical ),
        maxCount_(0),
        column_(0),
        row_(0)
    {}

    //! column alignments
    typedef QVector<Qt::Alignment> AlignmentList;

    //! set columns
    void setMaxCount( const int& max_count )
    {
        maxCount_ = max_count;
        columnAlignments_ = AlignmentList( max_count, (Qt::Alignment)0 );
    }

    //! set column alignment
    void setColumnAlignment( const int& column, const Qt::Alignment alignment )
    {
        assert( _boundCheck( column ) );
        columnAlignments_[column] = alignment;
    }

    //! set orientation
    void setOrientation( const Qt::Orientation& orientation )
    { orientation_ = orientation; }

    //! add widget
    void addWidget( QWidget* widget, int row, int column, Qt::Alignment alignment = 0 )
    { GridLayout::addWidget( widget, row, column, 1, 1, alignment ); }

    //! add widget
    void addWidget ( QWidget * widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 )
    {
        assert( rowSpan > 0 );
        assert( columnSpan > 0 );
        if( alignment == 0 && _boundCheck( column ) && columnSpan == 1 ) alignment = columnAlignments_[column];
        QGridLayout::addWidget( widget, row, column, rowSpan, columnSpan, alignment );
        setLocation( row+rowSpan-1, column+columnSpan-1 );
        _increment();
    }

    //! add widget
    void addWidget( QWidget* widget, Qt::Alignment alignment = 0 )
    {
        assert( maxCount_ > 0 );
        if( alignment == 0 &&  _boundCheck( column_ ) ) alignment = columnAlignments_[column_];
        QGridLayout::addWidget( widget, row_, column_, alignment );
        _increment();
    }

    //! add widget
    void addLayout( QLayout* layout, Qt::Alignment alignment = 0 )
    {
        assert( maxCount_ > 0 );
        if( alignment == 0 &&  _boundCheck( column_ ) ) alignment = columnAlignments_[column_];
        QGridLayout::addLayout( layout, row_, column_, alignment );
        _increment();
    }

    //! set current position in grid
    void setLocation( const int& row, const int& column )
    { row_ = row; column_ = column; }

    private:

    //! bound check
    bool _boundCheck( const int& column ) const
    { return column >= 0 && column < int( columnAlignments_.size() ); }

    //! increment from last position
    void _increment()
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

    //! orientation
    Qt::Orientation orientation_;

    //! number of columns
    int maxCount_;

    //! column alignments
    AlignmentList columnAlignments_;

    //! current column
    int column_;

    //! current row
    int row_;

};

#endif
