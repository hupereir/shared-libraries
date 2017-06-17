#ifndef TimeStamp_h
#define TimeStamp_h

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

#include <QDataStream>
#include <QDateTime>
#include <time.h>

//* time manipulation object
class TimeStamp:private Base::Counter<TimeStamp>
{

    public:

    //* default string for invalid timestamps
    static const QString TimeStampUnknown;

    //* empty creator
    explicit TimeStamp( void ):
        Counter( "TimeStamp" )
    {}

    //* time_t creator
    explicit TimeStamp( time_t time ):
        Counter( "TimeStamp" )
    { setTime( time ); }

    //* set time
    bool setTime( time_t time )
    {
        valid_ = time >= 0;
        time_ = time;
        if( valid_ ) tm_ = *localtime( &time );
        return valid_;
    }

    //* convert tm_ into time_
    bool makeTime( void )
    {
        valid_ = (time_ = mktime( &tm_ ) ) >= 0;
        return valid_;
    }

    //* inferior to operator
    bool operator < (const TimeStamp& stamp ) const
    { return( time_ < stamp.time_ ); }

    //* inferior to operator
    bool operator <= (const TimeStamp& stamp ) const
    { return( time_ <= stamp.time_ ); }

    //* inferior to operator
    bool operator > (const TimeStamp& stamp ) const
    { return( time_ > stamp.time_ ); }

    //* inferior to operator
    bool operator >= (const TimeStamp& stamp ) const
    { return( time_ >= stamp.time_ ); }

    //* equal to operator
    bool operator == (const TimeStamp& stamp ) const
    { return( time_ == stamp.time_ ); }

    //* different from operator
    bool operator != (const TimeStamp& stamp ) const
    { return( time_ != stamp.time_ ); }

    //*@name accessors
    //@{

    //* checks if timestamp is valid or not
    bool isValid( void ) const
    { return valid_; }

    //* timestamp format enumeration
    enum class Format
    {

        //* DD/MM/YYYY
        Date,

        //* YYYY/MM/DD
        DateUS,

        //* HH:MM
        Time,

        //* HH:MM:SS
        TimeLong,

        //* DD MONTH_string YYYY HH:MM:SS (week_day)
        Long,

        //* DD/MM/YYYY HH:MM
        Short,

        //* YYYY/MM/DD HH:MM
        ShortUS,

        //* DD_MM_YY
        DateTag,

        //* MONTH_string DD HH:MM
        JobTag

    };

    //* if timestamp is valid returns formated string
    QString toString( Format = Format::Short ) const;

    //* if timestamp is valid, returns formated string
    QString toString( Qt::DateFormat format ) const
    { return valid_ ? QDateTime::fromTime_t( time_ ).toString( format ) : TimeStampUnknown; }

    //* if timestamp is valid, returns formated string
    QString toString( const QString& format ) const
    { return valid_ ? QDateTime::fromTime_t( time_ ).toString( format ) : TimeStampUnknown; }

    //* returns time in second
    time_t unixTime( void ) const
    { return time_; }

    //* returns TimeStamp corresponding to _now_
    static TimeStamp now( void );

    //* retrieve seconds (between 0 and 59)
    int seconds( void ) const
    { return (valid_) ? tm_.tm_sec:0; }

    //* retrieve minutes (between 0 and 59)
    int minutes( void ) const
    { return (valid_) ? tm_.tm_min:0; }

    //* retrieve hour (between 0 and 23)
    int hours( void ) const
    { return (valid_) ? tm_.tm_hour:0; }

    //* retrieves day (between 1 and 31)
    int day( void ) const
    { return (valid_) ? tm_.tm_mday:0; }

    //* retrieves month (between 1 and 12)
    int month( void ) const
    { return (valid_) ? tm_.tm_mon+1:0; }

    //* retrieves year (between 1 and 12)
    int year( void ) const
    { return (valid_) ? tm_.tm_year+1900:0; }

    //* true if same day
    bool isSameDay( const TimeStamp& other ) const
    {
        return
            tm_.tm_mday == other.tm_.tm_mday &&
            tm_.tm_mon == other.tm_.tm_mon &&
            tm_.tm_year == other.tm_.tm_year;
    }

    //* true if same day
    bool isSameYear( const TimeStamp& other ) const
    { return tm_.tm_year == other.tm_.tm_year; }

    //@}

    //*@name modifiers
    //@{

    //* seconds (between 0 and 59)
    TimeStamp& setSeconds( int value )
    {
        tm_.tm_sec = value;
        return *this;
    }

    //* minutes (between 0 and 59)
    TimeStamp& setMinutes( int value )
    {
        tm_.tm_min = value;
        return *this;
    }

    //* hour (between 0 and 23)
    TimeStamp& setHours( int value )
    {
        tm_.tm_hour = value;
        return *this;
    }

    //* retrieves day (between 1 and 31)
    TimeStamp& setDay( int value )
    {
        tm_.tm_mday = value;
        return *this;
    }

    //* retrieves month (between 1 and 12)
    TimeStamp& setMonth( int value )
    {
        tm_.tm_mon = value - 1;
        return *this;
    }

    //* retrieves month (from string)
    TimeStamp& setMonth( const QString& );

    //* retrieves year (between 1 and 12)
    TimeStamp& setYear( int value )
    {
        tm_.tm_year = value - 1900;
        return *this;
    }

    //* tells if timestamp is valid or not
    TimeStamp& setValid( bool value )
    {
        valid_ = value;
        return *this;
    }

    //@}

    private:

    //* true if timestamp is properly set
    bool valid_ = false;

    //* unix time in second
    time_t time_ = -1;

    //* time structure from localtime()
    struct tm tm_;

    //*@name serializer
    //@{
    friend QDataStream& operator << ( QDataStream&, const TimeStamp& );
    friend QDataStream& operator >> ( QDataStream&, TimeStamp& );
    //@}

};

#endif
