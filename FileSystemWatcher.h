#ifndef FileSystemWatcher_h
#define FileSystemWatcher_h

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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*******************************************************************************/

#include "Counter.h"
#include "Debug.h"

#include <QBasicTimer>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QTimerEvent>

class FileSystemWatcher: public QFileSystemWatcher, public Counter
{
    Q_OBJECT

    public:

    //! constructor
    FileSystemWatcher( QObject* parent = 0 ):
        QFileSystemWatcher( parent ),
        Counter( "FileSystemWatcher" ),
        delay_( 500 )
    {
        Debug::Throw( "FileSystemWatcher::FileSystemWatcher.\n" );
        connect( this, SIGNAL( directoryChanged( const QString& ) ), SLOT( _addModifiedDirectory( const QString& ) ) );
    }

    //! destructor
    virtual ~FileSystemWatcher( void )
    {}

    //! delay
    void setDelay( int value )
    { delay_ = value; }

    signals:

    //! delayed directory changed signal
    void directoryChangedDelayed( const QString& );

    protected:

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    protected slots:

    //! add modified directory
    void _addModifiedDirectory( const QString& );

    private:

    //! delay
    int delay_;

    //! modified directories
    QStringList directories_;

    //! timer
    QBasicTimer timer_;

};

#endif
