#ifndef ValidFileThread_h
#define ValidFileThread_h

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

#include "Counter.h"
#include "FileRecord.h"
#include "base_qt_export.h"

#include <QMutex>
#include <QMutexLocker>
#include <QThread>

//* independent thread used to automatically save file
class BASE_QT_EXPORT ValidFileThread: public QThread, private Base::Counter<ValidFileThread>
{

    Q_OBJECT

    public:

    //* constructor
    explicit ValidFileThread( QObject* = nullptr );

    //* check duplicates
    void setCheckDuplicates( bool value )
    {
        QMutexLocker lock( &mutex_ );
        checkDuplicates_ = value;
    }

    //* set file
    void setRecords( const FileRecord::List& records )
    {
        QMutexLocker lock( &mutex_ );
        records_ = records;
    }

    Q_SIGNALS:

    //* records are available
    void recordsAvailable( const FileRecord::List&, bool );

    protected:

    //* Check files validity. Post a ValidFileEvent when finished
    void run() override;

    private:

    //* mutex
    QMutex mutex_;

    //* check duplicates
    bool checkDuplicates_ = true;

    //* list of records to be checked
    FileRecord::List records_;

};
#endif
