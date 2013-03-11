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
*
*******************************************************************************/

#include "Debug.h"
#include "Str.h"
#include "TimeStamp.h"
#include "Util.h"

#include <QObject>

//________________________________________________________________________________
const QString TimeStamp::STAMP_UNKNOWN( QObject::tr("unknown") );

//________________________________________________________________________________
TimeStamp TimeStamp::now( void )
{
    //Debug::Throw( "TimeStamp::now.\n" );
    TimeStamp out;
    out.time_ = time(0);
    out.tm_   = *localtime(&out.time_);
    out.valid_ = ( out.time_>=0 );
    return out;
}

//________________________________________________________________________________
QString TimeStamp::toString( TimeStamp::Format format ) const
{

    switch (format)
    {

        case DATE:
        return toString( "dd/MM/yyyy" );

        case DATE_US:
        return toString( "yyyy/MM/dd" );

        case TIME:
        return toString( "hh:mm" );

        case TIME_LONG:
        return toString( "hh:mm:ss" );

        case SHORT:
        return toString( "dd/MM/yyyy hh:mm" );

        case SHORT_US:
        return toString( "yyyy/MM/dd hh:mm" );

        case LONG:
        return toString( "dd MMM yyyy hh::mm::ss (ddd)" );

        case DATE_TAG:
        return toString( "dd_MM_yy" );

        case JOB_TAG:
        return toString( "MMM dd hh:mm" );

        default: return STAMP_UNKNOWN;

    }

}

//________________________________________________________________________________
TimeStamp& TimeStamp::setMonth( const QString& value )
{
    for( int index = 0; index < 12; ++index )
    {
        if( !value.compare( QDate::shortMonthName( index+1 ), Qt::CaseInsensitive ) )
        {
            tm_.tm_mon = index;
            break;
        }
    }

    return *this;

}

