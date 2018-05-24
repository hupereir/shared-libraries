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
void ErrorHandler::initialize()
{
    #if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    qInstallMessageHandler( _throw );
    #else
    qInstallMsgHandler( _throw );
    #endif
}


//_____________________________________________________________
ErrorHandler& ErrorHandler::get()
{
    static  ErrorHandler singleton;
    return singleton;
}

//_____________________________________________________________
void ErrorHandler::exit()
{
    #if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    qInstallMessageHandler( 0 );
    #else
    qInstallMsgHandler( 0 );
    #endif
}

//_____________________________________________________________
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
void ErrorHandler::_throw( QtMsgType type, const QMessageLogContext& context, const QString& message )
{

    const QString fullMessage = QString( "%1 file: %2 line: %3 function: %4" )
        .arg( message )
        .arg( context.file )
        .arg( context.line )
        .arg( context.function );
    _throw( type, qPrintable( fullMessage ) );

}
#endif

//_____________________________________________________________
void ErrorHandler::_throw( QtMsgType type, const char* message )
{
    QString localMessage( message );

    // check if message is to be disabled
    bool disabled = std::any_of( get().disabledMessages_.begin(), get().disabledMessages_.end(),
        [&localMessage]( const QString& message )
        { return localMessage.indexOf( message ) >= 0; } );

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
ErrorHandler::ErrorHandler()
{

    // install 'default' disabled messages
    disableMessage( "QSocketNotifier: invalid socket" );
    disableMessage( "QServerSocket: failed to bind or listen to the socket" );
    disableMessage( "QPixmap::resize: TODO: resize alpha data" );
    disableMessage( "QPainterPath::arcTo: Adding arc where a parameter is NaN, results are undefined" );
    disableMessage( "warning: Couldn't resolve property" );
    disableMessage( "QProcess: Destroyed while process is still running" );

    disableMessage( "QPainter::begin: Paint device returned engine == 0, type: 3" );
    disableMessage( "QPainter::setCompositionMode: Painter not active" );
    disableMessage( "QPainter::end: Painter not active, aborted" );
    disableMessage( "QCoreApplication::postEvent: Unexpected null receiver" );
    disableMessage( "Invalid entry (missing '=')" );
    disableMessage( "KConfigIni" );

    disableMessage( "Qt: Session management error: Could not open network socket" );

    // QT5 specific messages
    disableMessage( "void QWindow::setTransientParent" );
    disableMessage( "QBackingStore::flush() called with non-exposed window" );
    disableMessage( "QXcbClipboard: SelectionRequest too old" );
    disableMessage( "QXcbConnection: XCB error: 8 (BadMatch)" );

}
