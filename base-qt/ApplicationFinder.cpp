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

#include "ApplicationFinder.h"

#include "LineEditor.h"

#if QT_VERSION < 0x050000
#include <QProcessEnvironment>
#else
#include <QStandardPaths>
#endif

//_________________________________________________
ApplicationFinder::ApplicationFinder( QObject* parent ):
    QObject( parent ),
    Counter( "ApplicationFinder" )
{}

//_________________________________________________
void ApplicationFinder::add( File application, LineEditor* editor )
{ applications_.append( qMakePair( application, editor ) ); }

//_________________________________________________
void ApplicationFinder::update( void )
{

    // path list
    #if QT_VERSION < 0x050000
    File::List pathList;
    #endif

    for( const auto& application:applications_ )
    {

        // check current and do nothing if exists
        File current( application.second->text() );
        if( current.exists() ) continue;

        #if QT_VERSION < 0x050000
        if( pathList.empty() )
        {
            // get path list from environment
            auto environment = QProcessEnvironment::systemEnvironment();
            auto path = environment.value( "path" );
            if( path.isEmpty() ) path = environment.value( "PATH" );
            if( path.isEmpty() ) return;

            // split
            auto list = path.split(":", QString::SkipEmptyParts);
            for( const auto& item:list ) pathList.append( item );
            if( pathList.empty() ) return;
        }

        File found = application.first.find( pathList );
        #else
        File found = QStandardPaths::findExecutable( application.first );
        #endif

        if( !found.isEmpty() ) application.second->setText( found );

    }

}
