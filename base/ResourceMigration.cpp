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

#include "ResourceMigration.h"

#include "Debug.h"
#include "Util.h"

#include <QDir>

//_________________________________________________________
ResourceMigration::ResourceMigration( const File &source ):
    Counter( QStringLiteral("ResourceMigration") ),
    source_( source )
{ Debug::Throw( QStringLiteral("ResourceMigration::ResourceMigration.\n") ); }

//_________________________________________________________
bool ResourceMigration::migrate( const File &destination ) const
{

    // check source
    if( !source_.exists() )
    {
        Debug::Throw() << "ResourceMigration::migrate - file " << source_ << " does not exist" << endl;
        return false;
    }

    // check destination
    if( destination.exists() )
    {
        Debug::Throw() << "ResourceMigration::migrate - file " << destination << " already exists" << endl;
        if( source_.exists() ) source_.remove();
        return false;
    }

    // create destination directory
    if( !QDir().mkpath( destination.path() ) )
    {
        Debug::Throw() << "ResourceMigration::migrate - unable to create path " << destination.path() << endl;
        return false;
    }

    // copy source over destination
    if( source_.copy( destination ) )
    {

        source_.remove();
        Debug::Throw(0) << "ResourceMigration::migrate - migration from " << source_ << " to " << destination << " succesful" << endl;
        return true;

    } else {

        Debug::Throw() << "ResourceMigration::migrate - cannot copy " << source_ << " over " << destination << endl;
        return false;

    }

}
