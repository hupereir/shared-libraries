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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "ErrorHandler.h"
#include "Debug.h"

#include <QTextStream>
#include <cstdlib>

//_____________________________________________________________
void ErrorHandler::initialize( void )
{

    #if QT_VERSION >= 0x050000
    qInstallMessageHandler( _throw );
    #else
    qInstallMsgHandler( _throw );
    #endif
}


//_____________________________________________________________
ErrorHandler& ErrorHandler::get( void )
{
    static  ErrorHandler singleton;
    return singleton;
}

//_____________________________________________________________
#if QT_VERSION >= 0x050000
void ErrorHandler::_throw( QtMsgType type, const QMessageLogContext& context, const QString& message )
{

    QString fullMessage;
    QTextStream( &fullMessage ) << message << " file: " << context.file << " line: " << context.line << " function: " << context.function;
    _throw( type, fullMessage.toLatin1() );
}
#endif

//_____________________________________________________________
void ErrorHandler::_throw( QtMsgType type, const char* message )
{
    Debug::Throw() << "ErrorHandler::Throw - " << message << endl;

    QString localMessage( message );

    // check if message is to be disabled
    bool disabled( false );
    foreach( const QString& message, get()._disabledMessages() )
    {
        if( localMessage.indexOf( message ) >= 0 )
        {
            disabled = true;
            break;
        }
    }

    // check message type
    QTextStream what( stderr );
    switch ( type ) {

        case QtDebugMsg: break;

        case QtWarningMsg:
        if( !disabled ) what << "ErrorHandler::Throw - warning: " << message << endl;
        break;

        case QtFatalMsg:
        what << "ErrorHandler::Throw - fatal: " << message << endl;
        disabled = false;
        abort();
        break;

        default:
        if( !disabled ) what << "ErrorHandler::Throw - unknown: " << message << endl;
        break;

    }

    return;
}

//_______________________________________________________________
ErrorHandler::ErrorHandler( void )
{

    // install 'default' disabled messages
    disableMessage( "QSocketNotifier: invalid socket" );
    disableMessage( "QServerSocket: failed to bind or listen to the socket" );
    disableMessage( "QPixmap::resize: TODO: resize alpha data" );
    disableMessage( "QPainterPath::arcTo: Adding arc where a parameter is NaN, results are undefined" );
    disableMessage( "warning: Couldn't resolve property" );
    disableMessage( "QProcess: Destroyed while process is still running" );

    // QT5 specific messages
    disableMessage( "QBackingStore::flush() called with non-exposed window" );
    disableMessage( "QXcbClipboard: SelectionRequest too old" );

}
