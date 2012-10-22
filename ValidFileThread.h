#ifndef ValidFileThread_h
#define ValidFileThread_h

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

#include "Counter.h"
#include "FileRecord.h"

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

//! independent thread used to automatically save file
class ValidFileThread: public QThread, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    ValidFileThread( QObject* = 0 );

    //! check duplicates
    void setCheckDuplicates( bool value )
    {
        QMutexLocker lock( &mutex_ );
        checkDuplicates_ = value;
    }

    //! set file
    void setRecords( const FileRecord::List& records )
    {
        QMutexLocker lock( &mutex_ );
        records_ = records;
    }

    //! Check files validity. Post a ValidFileEvent when finished
    void run( void );

    signals:

    //! records are available
    void recordsAvailable( const FileRecord::List&, bool );

    private:

    //! mutex
    QMutex mutex_;

    //! check duplicates
    bool checkDuplicates_;

    //! list of records to be checked
    FileRecord::List records_;

};
#endif
