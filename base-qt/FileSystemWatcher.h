#ifndef FileSystemWatcher_h
#define FileSystemWatcher_h

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
#include "Debug.h"

#include <QBasicTimer>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QTimerEvent>

class FileSystemWatcher: public QFileSystemWatcher, private Base::Counter<FileSystemWatcher>
{
    Q_OBJECT

    public:

    //* constructor
    explicit FileSystemWatcher( QObject* parent = nullptr ):
        QFileSystemWatcher( parent ),
        Counter( QStringLiteral("FileSystemWatcher") )
    {
        Debug::Throw( QStringLiteral("FileSystemWatcher::FileSystemWatcher.\n") );
        connect( this, &QFileSystemWatcher::directoryChanged, this, &FileSystemWatcher::_addModifiedDirectory );
    }

    //* delay
    void setDelay( int value )
    { delay_ = value; }

    Q_SIGNALS:

    //* delayed directory changed signal
    void directoryChangedDelayed( const QString& );

    protected:

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    private:

    //* add modified directory
    void _addModifiedDirectory( const QString& );

    //* delay
    int delay_ = 500;

    //* modified directories
    QStringList directories_;

    //* timer
    QBasicTimer timer_;

};

#endif
