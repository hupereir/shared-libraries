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

#include "TextSeparator.h"

//___________________________________
const TextSeparator& TextSeparator::get()
{
    static TextSeparator singleton;
    return singleton;
}

//___________________________________
TextSeparator::TextSeparator()
{

    // initialize separator
    const QString baseSeparators( " \t\n" );
    for( int i=0; i< baseSeparators.size(); i++ )
    { baseSeparators_.insert( baseSeparators[i] ); }

    const QString extendedSeparators( ",;.:?!&+-=*/|-()'`{}[]<>\"\\%" );
    for( int i=0; i< extendedSeparators.size(); i++ )
    { extendedSeparators_.insert( extendedSeparators[i] ); }

    separators_ = baseSeparators_;
    separators_.unite( extendedSeparators_ );

}
