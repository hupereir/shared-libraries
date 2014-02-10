#ifndef _Word_h_
#define _Word_h_


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

#include "TextPosition.h"
#include <QSet>

namespace SpellCheck
{
    //! keep track of misspelled words in text
    class Word: public QString
    {
        public:

        //! shortcut for set
        typedef QSet<Word> Set;

        //! constructor
        Word( const QString& word = QString(), const int& position = 0 ):
            QString( word ),
            position_( position )
        {}

        //! equal to operator
        bool operator == ( const Word& word ) const
        { return position_ == word.position_; }

        //! less than to operator
        bool operator < ( const Word& word ) const
        { return position_ < word.position_; }

        //! position
        const int& position( void ) const
        { return position_; }

        //! true if given position is in the selected word
        bool has( const int& position ) const
        {
            return
                position >= position_ &&
                position < position_+static_cast<int>(size());
        }

        //! returns true if Word is find at position
        class AtPositionFTor
        {

            public:

            //! constructor
            AtPositionFTor( const int& position ):
                position_( position )
            {}

            //! predicate
            bool operator() (const Word& word )
            { return word.has( position_ ); }

            private:

            //! predicate position
            const int position_;

        };

        private:

        //! position in text
        const int position_;

    };
}

#endif
