#ifndef TextBlockRange_h
#define TextBlockRange_h

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

#include "base_qt_export.h"
#include <QTextBlock>
#include <QTextDocument>
#include <iterator>

//* block range
class BASE_QT_EXPORT TextBlockRange
{

    public:

    //* block iterator
    class BASE_QT_EXPORT iterator : public std::iterator<std::forward_iterator_tag, QTextBlock>
    {

        public:

        //* constructor
        explicit iterator()
        {}

        //* constructor
        explicit iterator( const QTextBlock& block ):
            block_( block )
        {}

        //* constructor
        explicit iterator( QTextBlock&& block ):
            block_( std::move( block ) )
        {}

        //*@ accessors
        //@{

        const QTextBlock& get() const
        { return block_; }

        //* dereference
        const QTextBlock& operator*() const
        { return block_; }

        //* dereference
        const QTextBlock* operator->() const
        { return &block_; }

        //@}

        //*@modifiers
        //@{

        QTextBlock& get()
        { return block_; }

        //* dereference
        QTextBlock& operator*()
        { return block_; }

        //* dereference
        QTextBlock* operator->()
        { return &block_; }

        //* prefix increment
        iterator& operator++()
        {
            block_ = block_.next();
            return *this;
        }

        //* postfix increment
        iterator operator++(int)
        {
            iterator tmp( *this );
            block_ = block_.next();
            return tmp;
        }

        //@}

        private:

        //* uncerlying block
        QTextBlock block_;

        //* different from operator
        friend bool operator == (const iterator& lhs, const iterator& rhs )
        { return lhs.block_ == rhs.block_; }

        //* different from operator
        friend bool operator != (const iterator& lhs, const iterator& rhs )
        { return lhs.block_ != rhs.block_; }

    };

    //* constructor
    TextBlockRange()
    {}

    //* constructor
    TextBlockRange( const QTextDocument& document ):
        begin_( document.begin() ),
        end_( document.end() )
    {}

    //* constructor
    TextBlockRange( QTextDocument* document ):
        begin_( document->begin() ),
        end_( document->end() )
    {}

    //* constructor from blocks
    TextBlockRange( const QTextBlock& begin, const QTextBlock& end ):
        begin_( begin ),
        end_( end )
    {}

    //* constructor from blocks
    TextBlockRange( QTextBlock&& begin, QTextBlock&& end ):
        begin_( std::move(begin) ),
        end_( std::move(end) )
    {}

    //* begin of the range
    iterator begin() const { return begin_; }

    //* end of the range
    iterator end() const { return end_; }

    private:

    iterator begin_;
    iterator end_;

};

#endif
