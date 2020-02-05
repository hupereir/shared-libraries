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

//* stores paragraph and index (in paragraph) into unique structure
class TextPosition final: private Base::Counter<TextPosition>
{
    public:

    //* default constructor
    explicit TextPosition( int paragraph = 0, int index = 0 ):
        Counter( QStringLiteral("TextPosition") ),
        paragraph_( paragraph ),
        index_( index )
    {}

    //* construct from document and absolute index
    explicit TextPosition( QTextDocument*, int index );

    //* paragraph
    int paragraph() const
    { return paragraph_; }

    //* paragraph
    int& paragraph()
    { return paragraph_; }

    //* index
    int index() const
    { return index_; }

    //* index
    int& index()
    { return index_; }

    //* get absolute index for given document
    int index( QTextDocument* ) const;

    //* validity
    bool isValid() const
    { return (paragraph() >= 0 && index() >= 0 ); }

    private:

    //* paragraph index
    int paragraph_ = 0;

    //* character index
    int index_ = 0;

    //* dump to stream
    friend QTextStream& operator << ( QTextStream& out, const TextPosition& position )
    {
        out << "(" << position.paragraph() << "," << position.index() << ")";
        return out;
    }

};

//* lower than operater
inline bool operator < (const TextPosition& first, const TextPosition& second)
{
    if( first.paragraph() != second.paragraph() ) return first.paragraph() < second.paragraph();
    else return first.index() < second.index();
}

//* equal operator
inline bool operator == (const TextPosition& first, const TextPosition& second)
{ return (first.paragraph() == second.paragraph() && first.index() == second.index()); }

#endif
