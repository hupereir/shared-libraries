#ifndef TextPosition_h
#define TextPosition_h

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

#include <QTextStream>
#include <QTextDocument>

//! stores paragraph and index (in paragraph) into unique structure
class TextPosition: public Counter
{
    public:

    //! default constructor
    TextPosition( int paragraph = 0, int index = 0 ):
        Counter( "TextPosition" ),
        paragraph_( paragraph ),
        index_( index )
    {}

    //! construct from document and absolute index
    TextPosition( QTextDocument*, int index );

    //! paragraph
    const int& paragraph( void ) const
    { return paragraph_; }

    //! paragraph
    int& paragraph( void )
    { return paragraph_; }

    //! index
    const int& index( void ) const
    { return index_; }

    //! index
    int& index( void )
    { return index_; }

    //! get absolute index for given document
    int index( QTextDocument* ) const;

    //! lower than operater
    bool operator < (const TextPosition& position ) const
    {
        if( paragraph() != position.paragraph() ) return paragraph() < position.paragraph();
        else return index() < position.index();
    }

    //! greater than operator
    bool operator > (const TextPosition& position ) const
    {
        if( paragraph() != position.paragraph() ) return paragraph() > position.paragraph();
        else return index() > position.index();
    }

    //! lower than operater
    bool operator <= (const TextPosition& position ) const
    { return !(*this > position ); }

    //! larger than operater
    bool operator >= (const TextPosition& position ) const
    { return !(*this < position ); }

    //! equal operator
    bool operator == (const TextPosition& position ) const
    { return (paragraph() == position.paragraph() && index() == position.index()); }

    //! equal operator
    bool operator != (const TextPosition& position ) const
    { return !( *this == position ); }

    //! validity
    bool isValid( void ) const
    { return (paragraph() >= 0 && index() >= 0 ); }

    private:

    //! paragraph index
    int paragraph_;

    //! character index
    int index_;

    //! dump to stream
    friend QTextStream& operator << ( QTextStream& out, const TextPosition& position )
    {
        out << "(" << position.paragraph() << "," << position.index() << ")";
        return out;
    }

};

#endif
