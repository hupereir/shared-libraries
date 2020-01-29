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
    qInstallMessageHandler( _throw );
}


//_____________________________________________________________
ErrorHandler& ErrorHandler::get()
{
    static  ErrorHandler singleton;
    return singleton;
}

//_____________________________________________________________
void ErrorHandler::exit()
{ qInstallMessageHandler(nullptr); }

//_____________________________________________________________
void ErrorHandler::_throw( QtMsgType type, const QMessageLogContext& context, const QString& message )
{

    const QString fullMessage = QString( "%1 file: %2 line: %3 function: %4" )
        .arg( message )
        .arg( context.file )
        .arg( context.line )
        .arg( context.function );

    // check if message is to be disabled
    bool disabled = std::any_of( get().disabledMessages_.begin(), get().disabledMessages_.end(),
        [&fullMessage]( const QString& message )
        { return fullMessage.indexOf( message ) >= 0; } );

    // check message type
    QTextStream what( stderr );
    switch ( type )
    {

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
ErrorHandler::ErrorHandler():
    disabledMessages_( {
    QStringLiteral( "QSocketNotifier: invalid socket"),
    QStringLiteral( "QServerSocket: failed to bind or listen to the socket"),
    QStringLiteral( "QPixmap::resize: TODO: resize alpha data"),
    QStringLiteral( "QPainterPath::arcTo: Adding arc where a parameter is NaN, results are undefined"),
    QStringLiteral( "warning: Couldn't resolve property"),
    QStringLiteral( "QProcess: Destroyed while process is still running"),

    QStringLiteral( "QPainter::begin: Paint device returned engine == 0, type: 3"),
    QStringLiteral( "QPainter::setCompositionMode: Painter not active"),
    QStringLiteral( "QPainter::end: Painter not active, aborted"),
    QStringLiteral( "QCoreApplication::postEvent: Unexpected null receiver"),
    QStringLiteral( "Invalid entry (missing '=')"),
    QStringLiteral( "KConfigIni"),

    QStringLiteral( "Qt: Session management error: Could not open network socket"),

    QStringLiteral( "void QWindow::setTransientParent"),
    QStringLiteral( "QBackingStore::flush() called with non-exposed window"),
    QStringLiteral( "QXcbClipboard: SelectionRequest too old"),
    QStringLiteral( "QXcbConnection: XCB error: 8 (BadMatch)" ) } )
{}
