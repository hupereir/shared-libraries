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

#include "Debug.h"
#include "TimeStamp.h"
#include "Util.h"

#include <QLocale>
#include <QObject>

//________________________________________________________________________________
const QString TimeStamp::TimeStampUnknown( QObject::tr("Unknown") );

//________________________________________________________________________________
TimeStamp TimeStamp::now()
{
    TimeStamp out;
    out.time_ = std::time(0);
    out.tm_   = *std::localtime(&out.time_);
    out.valid_ = ( out.time_>=0 );
    return out;
}

//________________________________________________________________________________
QString TimeStamp::toString( TimeStamp::Format format ) const
{

    switch (format)
    {

        case Format::Date:
        return toString( QStringLiteral("dd/MM/yyyy") );

        case Format::DateUS:
        return toString( QStringLiteral("yyyy/MM/dd") );

        case Format::Time:
        return toString( QStringLiteral("hh:mm") );

        case Format::TimeLong:
        return toString( QStringLiteral("hh:mm:ss") );

        case Format::Short:
        return toString( QStringLiteral("dd/MM/yyyy hh:mm") );

        case Format::ShortUS:
        return toString( QStringLiteral("yyyy/MM/dd hh:mm") );

        case Format::Long:
        return toString( QStringLiteral("dd MMM yyyy hh::mm::ss (ddd)") );

        case Format::DateTag:
        return toString( QStringLiteral("yyyyMMdd") );

        case Format::JobTag:
        return toString( QStringLiteral("MMM dd hh:mm") );

        default: return TimeStampUnknown;

    }

}

//________________________________________________________________________________
TimeStamp& TimeStamp::setMonth( const QString& value )
{
    const QLocale locale;
    for( int index = 0; index < 12; ++index )
    {
        if( !value.compare( locale.monthName( index+1, QLocale::ShortFormat ), Qt::CaseInsensitive ) )
        {
            tm_.tm_mon = index;
            break;
        }
    }

    return *this;

}

//________________________________________________________________________________
QDataStream& operator << ( QDataStream& stream, const TimeStamp& timestamp )
{
    static const quint32 version = 0;
    stream << version << quint64( timestamp.time_ );
    return stream;
}

//________________________________________________________________________________
QDataStream& operator >> ( QDataStream& stream, TimeStamp& timestamp )
{
    quint32 version;
    stream >> version;
    if( version == 0 )
    {
        quint64 time;
        stream >> time;
        timestamp.setTime( time );

    } else Debug::Throw(0) << "Unrecognized TimeStamp version: " << version << endl;
    return stream;
}
