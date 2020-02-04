#ifndef RegExpUtil_h
#define RegExpUtil_h
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

#include <QRegularExpression>

namespace Base
{
    template<class T>
    inline bool exactMatch( const QRegularExpression& regexp, const T& text )
    {
        const auto match( regexp.match( text ) );
        return match.hasMatch() && match.capturedLength() == text.size();
    }

    template<class T>
    inline void setCaseSensitivity( T& regexp, bool caseSensitive )
    {
        if( caseSensitive ) regexp.setPatternOptions( regexp.patternOptions() & ~QRegularExpression::CaseInsensitiveOption );
        else regexp.setPatternOptions( regexp.patternOptions()|QRegularExpression::CaseInsensitiveOption );
    }
}
#endif
