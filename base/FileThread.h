#ifndef FileThread_h
#define FileThread_h

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
#include "File.h"

#include <QMutex>
#include <QMutexLocker>
#include <QThread>

//! independent thread used to automatically save file
class FileThread: public QThread, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    FileThread( QObject* = 0 );

    //! command
    enum class Command
    {
        None,
        List,
        ListRecursive,
        SizeRecursive,
        Remove,
        Copy
    };

    //!@name accessors
    //@{

    //! command
    Command command( void ) const
    { return command_; }

    //! file
    File file( void ) const
    { return file_; }

    //! destination
    File destination( void ) const
    { return destination_; }

    //!flags
    File::ListFlags flags( void )
    { return flags_; }

    //! error state
    bool hasError( void ) const
    { return error_; }

    //! error string
    QString errorString( void )
    { return errorString_; }

    //@}

    //!@name modifiers
    //@{

    //! command
    void setCommand( Command command )
    {
        QMutexLocker lock( &mutex_ );
        command_ = command;
    }

    //! flags
    void setFlags( File::ListFlags flags )
    { flags_ = flags; }

    //! file
    void setFile( const File& file )
    {
        QMutexLocker lock( &mutex_ );
        file_ = file;
    }

    //! destination
    void setDestination( const File& destination )
    {
        QMutexLocker lock( &mutex_ );
        destination_ = destination;
    }

    //@}

    Q_SIGNALS:

    //! files are available
    void filesAvailable( const File::List& );

    //! size available
    void sizeAvailable( qint64 );

    protected:

    //! Check files validity. Post a ValidFileEvent when finished
    virtual void run( void );

    //! list files
    void _listFiles( const File& );

    //! update total size
    bool _updateTotalSize( void );

    //! compute total size
    void _computeTotalSize( void );

    private:

    //! mutex
    QMutex mutex_;

    //! command
    Command command_ = Command::None;

    //! flags
    File::ListFlags flags_ = File::ListFlag::None;

    //! file
    File file_;

    //! destination
    File destination_;

    //! current list of files
    File::List files_;

    //! full list of files
    File::List filesRecursive_;

    //! total size
    qint64 totalSize_ = 0;

    //! error
    bool error_ = false;

    //! error string
    QString errorString_;

};

#endif
