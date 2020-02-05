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
#include "Functors.h"
#include "TextFormat.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QTextStream>

namespace TextFormat
{
    class Block: private Base::Counter<Block>
    {

        public:

        //* constructor
        explicit Block( int begin = 0, int end = 0, Flags format = 0, const QColor& color = QColor() ):
            Counter( QStringLiteral("TextFormat::Block") ),
            begin_( begin ),
            end_( end ),
            format_( format ),
            color_( color )
        {}

        //* destructor
        virtual ~Block() = default;

        //*@name accessors
        //@{

        //* true if format info is empty
        bool isEmpty() const
        { return begin_ == end_; }

        //* begin position
        int begin() const
        { return begin_; }

        //* end position
        int end() const
        { return end_; }

        //* format
        Flags format() const
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
        void setFormat( Flags value )
        { format_ = value; }

        //* color
        void setColor( const QColor& value )
        { color_ = value; }

        //* unset color (replace by invalid)
        void unsetColor()
        { color_ = QColor(); }

        //* href
        void setHRef( const QString& value )
        { href_ = value; }

        //@}

        //* get block matching a given begin position
        using SamePositionBeginFTor = Base::Functor::Unary<Block, int, &Block::begin>;

        //* get block matching a given end position
        using SamePositionEndFTor = Base::Functor::Unary<Block, int, &Block::end>;

        //* get block containing a given position
        class ContainsFTor
        {

            public:

            //* constructor
            explicit ContainsFTor( int position ):
                position_( position )
            {}

            //* predicate
            bool operator() (const Block& format ) const
            { return position_ >= format.begin() && position_ < format.end(); }

            private:

            //* predicted position
            int position_ = 0;

        };


        //* format list
        using List = QList<Block>;

        private:

        //* starting range
        int begin_ = 0;

        //* ending range
        int end_ = 0;

        //* format (a bitwise or of the format bits above)
        Flags format_ = 0;

        //* color
        QColor color_;

        //* href
        QString href_;

        //* streamer
        friend QTextStream& operator << ( QTextStream& out, const Block& format )
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
        friend QTextStream& operator << ( QTextStream& out, const Block::List& formats )
        {
            out << "TextFormatList: " << endl;
            for( const auto& block:formats )
            { out << block; }
            out << endl;
            return out;
        }

    };

    //* less than operator (based on Begin position)
    inline bool operator < (const Block& first, const Block& second )
    { return first.begin() < second.begin(); }

}

#endif
