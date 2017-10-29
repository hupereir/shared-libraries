#ifndef CustomProcess_h
#define CustomProcess_h

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
#include "Command.h"
#include "ObjectDeleter.h"

#include <QProcess>
#include <QObject>

#include <memory>

//* customized process to store stdout/stderr in full/flushed streams
class CustomProcess: public QProcess, private Base::Counter<CustomProcess>
{

    Q_OBJECT

    public:

    //* creator
    explicit CustomProcess( QObject* = nullptr );

    //* destructor
    ~CustomProcess() override;

    //* smart pointer
    using Pointer = std::unique_ptr<CustomProcess, Base::ObjectDeleter>;

    /**
    \brief
    add arguments.
    every space separated argument is added separately
    */
    void start( QString arguments, OpenMode mode = ReadWrite );

    //* run
    void start( const QStringList&, OpenMode = ReadWrite );

    //* run
    void start( const Base::Command& command, OpenMode mode = ReadWrite )
    { start( command.get(), mode ); }

    //* ensure object is deleted at job completion
    void setAutoDelete();

    //* error message
    static QString errorMessage( QProcess::ProcessError error );

};

#endif
