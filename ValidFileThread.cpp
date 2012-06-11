// $Id$

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*******************************************************************************/

/*!
\file ValidFileThread.cpp
\brief check validity of a set of files
\author  Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <algorithm>
#include <QApplication>
#include "ValidFileThread.h"
#include "File.h"



//______________________________________________________
QEvent::Type ValidFileEvent::eventType( void )
{

    #if QT_VERSION >= 0x040400
    static QEvent::Type event_type = (QEvent::Type) QEvent::registerEventType();
    #else
    static QEvent::Type event_type = QEvent::User;
    #endif

    return event_type;

}

//______________________________________________________
void ValidFileThread::run( void )
{

    bool hasInvalidRecords( false );

    // loop over files, check if exists, set validity accordingly, and post event
    for( FileRecord::List::iterator iter = records_.begin(); iter != records_.end(); ++iter )
    {
        iter->setValid( File( iter->file() ).exists() );
        hasInvalidRecords |= !iter->isValid();
    }

    // look for duplicated records
    if( _checkDuplicates() )
    {
        for( FileRecord::List::iterator iter = records_.begin(); iter != records_.end(); )
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

    qApp->postEvent( reciever_, new ValidFileEvent( records_, hasInvalidRecords ) );
    return;

}
