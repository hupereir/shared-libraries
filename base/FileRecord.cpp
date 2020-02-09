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

#include "FileRecord.h"

//_______________________________________________
const QString FileRecord::MimeType( QStringLiteral("internal/file-record-list") );

//_______________________________________________
FileRecord::PropertyId::IdMap& FileRecord::PropertyId::_idMap()
{
    static FileRecord::PropertyId::IdMap idMap;
    return idMap;
}

//_______________________________________________
FileRecord::PropertyId::NameMap& FileRecord::PropertyId::_nameMap()
{
    static FileRecord::PropertyId::NameMap nameMap;
    return nameMap;
}

//_______________________________________________
FileRecord::PropertyId::Id& FileRecord::PropertyId::_counter()
{
    static Id counter(0);
    return counter;
}

//_______________________________________________
FileRecord::PropertyId::Id FileRecord::PropertyId::get( const QString& name )
{

    // see if iterator exists in list
    IdMap::const_iterator iter;
    if( ( iter = _idMap().find( name ) ) != _idMap().end() )
    { return iter.value(); }

    // insert otherwise, increment counter and return proper value
    _idMap().insert( name, _counter() );
    _nameMap().append(name);
    _counter()++;

    return _counter()-1;

}

//_______________________________________________
QString FileRecord::PropertyId::get( FileRecord::PropertyId::Id id )
{ return _nameMap()[id]; }

//_______________________________________________
bool operator < (const FileRecord& first, const FileRecord& second )
{ return first.file() < second.file(); }

//_______________________________________________
bool operator == (const FileRecord& first, const FileRecord& second )
{ return first.file() == second.file(); }

//_______________________________________________
FileRecord& FileRecord::addProperty( PropertyId::Id id, const QString& value )
{

    properties_[ id ] = value;
    return *this;
}
