// $Id$

/***********************************************************************
*
* Copyright 2013 Hugo PEREIRA DA COSTA <hugo.pereira@free.fr>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#include "CommandLineArguments.h"
#include "Debug.h"

//_____________________________________________________________________
CommandLineArguments::CommandLineArguments( int argc, char* argv[] ):
Counter( "CommandLineArguments" )
{
    Debug::Throw( "CommandLineArguments::CommandLineArguments.\n" );
    for( int i=0; i<argc; i++ )
    { if( argv[i] && strlen( argv[i] ) ) append( argv[i] ); }

}

//_____________________________________________________________________
CommandLineArguments::CommandLineArguments( const QStringList& ref ):
    QStringList( ref ),
    Counter( "CommandLineArguments" )
{}

//_____________________________________________________________________
char** CommandLineArguments::argv( void ) const
{
    char** out = new char*[argc()];
    int count(0);
    for( QStringList::const_iterator iter = begin(); iter != end(); ++iter, count++ )
    {
        char* arg = new char[iter->size()+1];
        sprintf( arg, "%s", qPrintable( *iter ) );
        out[count] = arg;
    }

    return out;
}
