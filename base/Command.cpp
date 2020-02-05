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

#include "Command.h"

#include "CppUtil.h"
#include "Debug.h"

#include <QProcess>
#include <QRegularExpression>

namespace Base
{

    //____________________________________________
    bool Command::run( const QString& path ) const
    {
        Debug::Throw( QStringLiteral("Command::run.\n") );
        if( values_.empty() ) return false;

        const auto program( values_.front() );
        auto arguments( values_ );
        arguments.removeFirst();
        return QProcess::startDetached( program, arguments, path );
    }

    //_________________________________________________________
    QStringList Command::_parse( const QString &in )
    {

        Debug::Throw( QStringLiteral("Command::parse.\n") );
        QStringList out;

        // first split using '"' as separator to get "single string" arguments that contain strings
        // split strings that are not enclosed into quotes using white spaces as separator
        // by construction, the first part is not enclosed into quotes; the second is; the third is not, etc.
        bool split( true );
        for( const auto& line:in.split( QLatin1Char('\"'), QString::KeepEmptyParts ) )
        {
            if( !line.isEmpty() )
            {
                static const QRegularExpression regExp( QStringLiteral("\\s+") );
                if( split ) out.append( line.split( regExp, QString::SkipEmptyParts ) );
                else out.append( QString("\"") + line + "\"" );
            }

            split = !split;
        }

        // print output
        if( Debug::level() >= 1 )
        {
            for( const auto& line:out )
            { Debug::Throw(0) << "Command::parse - \"" << line << "\"" << endl; }
        }

        return out;

    }

}
