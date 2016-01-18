#ifndef TextSeparator_h
#define TextSeparator_h

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

#include <QSet>
#include <QString>

//! text separator between words
class TextSeparator
{

    public:

    //! shortcut to set of separators
    using SeparatorSet = QSet< QChar >;

    //! return singleton
    static const TextSeparator& get( void );

    //! retrieve base separators
    const SeparatorSet& base() const
    { return baseSeparators_; }

    //! retrieve base separators
    const SeparatorSet& extended() const
    { return extendedSeparators_; }

    //! retrieve all separators
    const SeparatorSet& all() const
    { return separators_; }

    private:

    //! constructor
    TextSeparator( void );

    //! base separators (space, tab, end of line)
    SeparatorSet baseSeparators_;

    //! extended separators (brackets, parenthesis, quotes, etc.)
    SeparatorSet extendedSeparators_;

    //! all separators
    SeparatorSet separators_;

};

#endif
