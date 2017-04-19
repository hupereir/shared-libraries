#ifndef TextFormatBlock_h
#define TextFormatBlock_h

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
#include "TextFormat.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QTextStream>

namespace Format
{
    //* text format
    /*!
    starting with Qt4, text blocks are no longer referenced by paragraph and index separately, but
    by an absolute index in document. However one must keep the possibility to handle paragraph id
    and convert them to absolute indexes in order to read old formats generated with Qt3.
    As a result parBegin and parEnd are still available but cannot be modified from outside of the class.
    */
    class TextFormatBlock: public Counter
    {

        public:

        //* constructor
        TextFormatBlock( int begin = 0, int end = 0, TextFormatFlags format = 0, const QColor& color = QColor() ):
            Counter( "TextFormatBlock" ),
            begin_( begin ),
            end_( end ),
            format_( format ),
            color_( color )
        {}

        //* less than operator (based on Begin position)
        bool operator < (const TextFormatBlock& format ) const
        { return begin_ < format.begin_; }

        //*@name accessors
        //@{

        //* true if format info is empty
        bool isEmpty( void ) const
        { return begin_ == end_; }

        //* begin position
        int begin() const
        { return begin_; }

        //* end position
        int end() const
        { return end_; }

        //* format
        TextFormatFlags format() const
        { return format_; }

        //* color
        const QColor& color() const
        { return color_; }

        //* href
        const QString& href() const
        { return href_; }

        //@}

        //*@name modifiers
        //@{

        //* begin position
        void setBegin( int value )
        { begin_ = value; }

        //* end position
        void setEnd( int value )
        { end_ = value; }

        //* format
        void setFormat( TextFormatFlags value )
        { format_ = value; }

        //* color
        void setColor( const QColor& value )
        { color_ = value; }

        //* unset color (replace by invalid)
        void unsetColor( void )
        { color_ = QColor(); }

        //* href
        void setHRef( const QString& value )
        { href_ = value; }

        //@}

        //* get TextFormatBlock matching a given begin position
        class SamePositionBeginFTor
        {

            public:

            //* constructor
            SamePositionBeginFTor( const int& position ):
                position_( position )
            {}

            //* predicate
            bool operator() (const TextFormatBlock& format ) const
            { return format.begin() == position_; }

            private:

            //* predicted position
            int position_ = 0;

        };

        //* get TextFormatBlock matching a given end position
        class SamePositionEndFTor
        {

            public:

            //* constructor
            SamePositionEndFTor( const int& position ):
                position_( position )
            {}

            //* predicate
            bool operator() (const TextFormatBlock& format ) const
            { return format.end() == position_; }

            private:

            //* predicted position
            int position_ = 0;

        };

        //* get TextFormatBlock containing a given position
        class ContainsFTor
        {

            public:

            //* constructor
            ContainsFTor( const int& position ):
                position_( position )
            {}

            //* predicate
            bool operator() (const TextFormatBlock& format ) const
            { return position_ >= format.begin() && position_ < format.end(); }

            private:

            //* predicted position
            int position_ = 0;

        };


        //* format list
        using List = QList<Format::TextFormatBlock>;

        private:

        //* starting range
        int begin_ = 0;

        //* ending range
        int end_ = 0;

        //* format (a bitwise or of the format bits above)
        TextFormatFlags format_ = 0;

        //* color
        QColor color_;

        //* href
        QString href_;

        //* streamer
        friend QTextStream& operator << ( QTextStream& out, const TextFormatBlock& format )
        {
            out
                << "begin: " << format.begin()
                << " end: " << format.end()
                << " format: " << format.format()
                << " color: " << format.color().name();

            if( !format.href().isEmpty() ) out << " href: " << format.href();
            out << endl;
            return out;
        }

        //* streamer
        friend QTextStream& operator << ( QTextStream& out, const TextFormatBlock::List& formats )
        {
            out << "TextFormatList: " << endl;
            for( const auto& block:formats )
            { out << block; }
            out << endl;
            return out;
        }

    };


};

#endif
