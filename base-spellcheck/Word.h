#ifndef Word_h
#define Word_h


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

#include "base_spellcheck_export.h"
#include "TextPosition.h"
#include <QSet>

namespace SpellCheck
{
    //* keep track of misspelled words in text
    class BASE_SPELLCHECK_EXPORT Word final
    {
        public:

        //* shortcut for set
        using Set = QSet<Word>;

        //* constructor
        explicit Word( const QString& word = QString(), int position = 0 ):
            value_( word ),
            position_( position )
        {}

        //*@name accessors
        //@{

        //* accessor
        const QString& get() const { return value_; }

        //* position
        int position() const
        { return position_; }

        //* true if given position is in the selected word
        bool has( int position ) const
        {
            return
                position >= position_ &&
                position < position_+static_cast<int>(value_.size());
        }

        //* empty
        bool isEmpty() const { return value_.isEmpty(); }

        //* length
        int length() const { return value_.length(); }

        //@}

        //* returns true if Word is find at position
        class BASE_SPELLCHECK_EXPORT AtPositionFTor
        {

            public:

            //* constructor
            explicit AtPositionFTor( int position ):
                position_( position )
            {}

            //* predicate
            bool operator() (const Word& word )
            { return word.has( position_ ); }

            private:

            //* predicate position
            const int position_;

        };

        private:

        //* value
        QString value_;

        //* position in text
        const int position_ = 0;

    };

    //* equal to operator
    inline bool operator == ( const Word& first, const Word& second )
    { return first.position() == second.position(); }

    //____________________________________________________
    inline uint qHash( const Word& word ) { return qHash( word.get() ); }

}

#endif
