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

/*
important note:
customProcess must _not_ call Debug method because underlying multithreading
might cause the application to hang, notably when process crashes
*/

#include "CustomProcess.h"

#include <QRegularExpression>

//____________________________________________________
CustomProcess::CustomProcess( QObject* parent ):
QProcess( parent ),
Counter( QStringLiteral("CustomProcess") )
{}

//____________________________________________________
CustomProcess::~CustomProcess()
{ if( state() != QProcess::NotRunning ) kill(); }

//____________________________________________________
void CustomProcess::start( const QString &arguments, OpenMode mode )
{
    static const QRegularExpression regExp(QStringLiteral("\\s"));
    start( arguments.split( regExp, Qt::SkipEmptyParts ), mode );
}

//____________________________________________________
void CustomProcess::start( const QStringList& arguments, OpenMode mode )
{

    if( arguments.empty() ) return;

    QString program( arguments.front() );
    auto localArgs( arguments );
    localArgs.removeFirst();
    QProcess::start( program, localArgs,  mode );

}

//______________________________________________________________
void CustomProcess::setAutoDelete()
{ connect( this, QOverload<int,QProcess::ExitStatus>::of( &CustomProcess::finished ), [this](int,QProcess::ExitStatus) { deleteLater(); } ); }

//______________________________________________________________
QString CustomProcess::errorMessage( ProcessError error )
{
    switch( error )
    {

        case FailedToStart: return tr("Process failed to start");
        case Crashed: return tr("Process crashed");
        case Timedout: return tr("Process timed out");
        case WriteError: return tr("Process ended due to write error");
        case ReadError: return tr("Process ended du to read error");
        default: return tr("Process ended with unknown error");
    }

}
