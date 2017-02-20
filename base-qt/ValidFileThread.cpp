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

#include "ValidFileThread.h"
#include "File.h"

#include <QMetaType>
#include <algorithm>

//______________________________________________________
ValidFileThread::ValidFileThread( QObject* parent ):
    QThread( parent ),
    Counter( "ValidFileThread" )
{
    // register FileRecord::List as meta type so that it can be used in SIGNAL
    qRegisterMetaType<FileRecord::List>( "FileRecord::List" );
}

//______________________________________________________
void ValidFileThread::run( void )
{

    QMutexLocker lock( &mutex_ );
    bool hasInvalidRecords( false );

    // loop over files, check if exists, set validity accordingly, and post event
    for( auto& record:records_ )
    {
        record.setValid( File( record.file() ).exists() );
        hasInvalidRecords |= !record.isValid();
    }

    // look for duplicated records
    if( checkDuplicates_ )
    {
        for( auto&& iter = records_.begin(); iter != records_.end(); )
        {

            // check item validity
            if( iter->isValid() )
            {

                // check for duplicates
                FileRecord& current( *iter );
                FileRecord::SameCanonicalFileFTor ftor( current.file() );
                if( std::find_if( ++iter, records_.end(), ftor ) != records_.end() )
                {
                    current.setValid( false );
                    hasInvalidRecords = true;
                }

            } else { ++iter; }

        }

    }

    emit recordsAvailable( records_, hasInvalidRecords );

    return;

}
