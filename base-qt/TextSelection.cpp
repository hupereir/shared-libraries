
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

#include "TextSelection.h"

//________________________________________________________________________________________
QTextStream &operator << (QTextStream &out,const TextSelection &selection)
{
    out
        << "TextSelection -"
        << " text: " << selection.text()
        << " replacement: " << selection.replaceText()
        << " highlight all: " << (selection.hasFlag(TextSelection::HighlightAll) ? "true":"false" )
        << " backward: " << (selection.hasFlag(TextSelection::Backward) ? "true":"false")
        << " case sensitive: " << (selection.hasFlag(TextSelection::CaseSensitive) ? "true":"false")
        << " entire word: " << (selection.hasFlag(TextSelection::EntireWord) ? "true":"false")
        << " regex: " << (selection.hasFlag(TextSelection::RegExp) ? "true":"false")
        << " no increment: " << (selection.hasFlag(TextSelection::NoIncrement) ? "true":"false");
    return out;
}

//________________________________________________________________________________________
bool operator == ( const TextSelection& lhs, const TextSelection& rhs)
{
    return
        lhs.text_ == rhs.text_ &&
        lhs.replaceText_ == rhs.replaceText_ &&
        lhs.flags_ == rhs.flags_;
}
