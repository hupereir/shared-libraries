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
    Counter( QStringLiteral("ValidFileThread") )
{ qRegisterMetaType<FileRecord::List>( "FileRecord::List" ); }

//______________________________________________________
void ValidFileThread::run()
{

    QMutexLocker lock( &mutex_ );
    bool hasInvalidRecords( false );

    // loop over files, check if exists, set validity accordingly, and post event
    for( auto& record:records_ )
    {
        record.setValid( record.file().exists() );
        hasInvalidRecords |= !record.isValid();
    }

    // look for duplicated records
    if( checkDuplicates_ && records_.size() > 1 )
    {

        std::sort( records_.begin(), records_.end(), FileRecord::CanonicalFileFTor() );
        auto&& iter = records_.begin();
        auto&& previous = iter++;
        for( ; iter != records_.end(); ++iter )
        {
            if( iter->isValid() && iter->canonicalFile() == previous->canonicalFile() )
            {
                iter->setValid( false );
                hasInvalidRecords = true;
            } else previous = iter;
        }

    }

    emit recordsAvailable( records_, hasInvalidRecords );

    return;

}
