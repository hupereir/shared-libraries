#ifndef TimeStamp_h
#define TimeStamp_h

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

#include "Counter.h"

#include <QDateTime>
#include <time.h>

//! time manipulation object
class TimeStamp:public Counter
{

    public:

    //! default string for invalid timestamps
    static const QString STAMP_UNKNOWN;

    //! empty creator
    TimeStamp( void ):
        Counter( "TimeStamp" ),
        valid_( false ),
        time_( -1 )
    {}

    //! time_t creator
    TimeStamp( time_t time ):
        Counter( "TimeStamp" )
    { setTime( time ); }

    //! set time
    virtual bool setTime( time_t time )
    {
        valid_ = time >= 0;
        time_ = time;
        if( valid_ ) tm_ = *localtime( &time );
        return valid_;
    }

    //! convert tm_ into time_
    virtual bool makeTime( void )
    {
        valid_ = (time_ = mktime( &tm_ ) ) >= 0;
        return valid_;
    }

    //! inferior to operator
    virtual bool operator < (const TimeStamp& stamp ) const
    { return( time_ < stamp.time_ ); }

    //! inferior to operator
    virtual bool operator >= (const TimeStamp& stamp ) const
    { return( time_ >= stamp.time_ ); }

    //! inferior to operator
    virtual bool operator > (const TimeStamp& stamp ) const
    { return( time_ > stamp.time_ ); }

    //! inferior to operator
    virtual bool operator == (const TimeStamp& stamp ) const
    { return( time_ == stamp.time_ ); }

    //!@name accessors
    //@{

    //! checks if timestamp is valid or not
    virtual bool isValid( void ) const
    { return valid_; }

    //! used to have fast access to the integer value
    virtual operator int (void) const
    { return int( time_ ); }

    //! timestamp format enumeration
    enum Format {

        //! DD/MM/YYYY
        DATE,

        //! YYYY/MM/DD
        DATE_US,

        //! HH:MM
        TIME,

        //! HH:MM:SS
        TIME_LONG,

        //! DD MONTH_string YYYY HH:MM:SS (week_day)
        LONG,

        //! DD/MM/YYYY HH:MM
        SHORT,

        //! YYYY/MM/DD HH:MM
        SHORT_US,

        //! DD_MM_YY
        DATE_TAG,

        //! MONTH_string DD HH:MM
        JOB_TAG
    };

    //! if timestamp is valid returns formated string
    virtual QString toString( Format = SHORT ) const;

    //! if timestamp is valid, returns formated string
    virtual QString toString( Qt::DateFormat format ) const
    { return valid_ ? QDateTime::fromTime_t( time_ ).toString( format ) : STAMP_UNKNOWN; }

    //! if timestamp is valid, returns formated string
    virtual QString toString( const QString& format ) const
    { return valid_ ? QDateTime::fromTime_t( time_ ).toString( format ) : STAMP_UNKNOWN; }

    //! returns time in second
    virtual time_t unixTime( void ) const
    { return time_; }

    //! returns TimeStamp corresponding to _now_
    static TimeStamp now( void );

    //! retrieve seconds (between 0 and 59)
    virtual int seconds( void ) const
    { return (valid_) ? tm_.tm_sec:0; }

    //! retrieve minutes (between 0 and 59)
    virtual int minutes( void ) const
    { return (valid_) ? tm_.tm_min:0; }

    //! retrieve hour (between 0 and 23)
    virtual int hours( void ) const
    { return (valid_) ? tm_.tm_hour:0; }

    //! retrieves day (between 1 and 31)
    virtual int day( void ) const
    { return (valid_) ? tm_.tm_mday:0; }

    //! retrieves month (between 1 and 12)
    virtual int month( void ) const
    { return (valid_) ? tm_.tm_mon+1:0; }

    //! retrieves year (between 1 and 12)
    virtual int year( void ) const
    { return (valid_) ? tm_.tm_year+1900:0; }

    //! true if same day
    bool isSameDay( const TimeStamp& other ) const
    {
        return
            tm_.tm_mday == other.tm_.tm_mday &&
            tm_.tm_mon == other.tm_.tm_mon &&
            tm_.tm_year == other.tm_.tm_year;
    }

    //@}

    //!@name modifiers
    //@{

    //! seconds (between 0 and 59)
    virtual TimeStamp& setSeconds( int value )
    {
        tm_.tm_sec = value;
        return *this;
    }

    //! minutes (between 0 and 59)
    virtual TimeStamp& setMinutes( int value )
    {
        tm_.tm_min = value;
        return *this;
    }

    //! hour (between 0 and 23)
    virtual TimeStamp& setHours( int value )
    {
        tm_.tm_hour = value;
        return *this;
    }

    //! retrieves day (between 1 and 31)
    virtual TimeStamp& setDay( int value )
    {
        tm_.tm_mday = value;
        return *this;
    }

    //! retrieves month (between 1 and 12)
    virtual TimeStamp& setMonth( int value )
    {
        tm_.tm_mon = value - 1;
        return *this;
    }

    //! retrieves month (from string)
    virtual TimeStamp& setMonth( const QString& );

    //! retrieves year (between 1 and 12)
    virtual TimeStamp& setYear( int value )
    {
        tm_.tm_year = value - 1900;
        return *this;
    }

    //! tells if timestamp is valid or not
    virtual TimeStamp& setValid( bool value )
    {
        valid_ = value;
        return *this;
    }

    //@}

    private:

    //! true if timeStamp is properly set
    bool valid_;

    //! unix time in second
    time_t time_;

    //! time structure from localtime()
    struct tm tm_;

};

#endif
