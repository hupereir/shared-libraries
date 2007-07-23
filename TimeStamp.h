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

/*!
   \file TimeStamp.h
   \brief time manipulation object
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <iostream>
#include <string>
#include <list>
#include <time.h>

#include "Counter.h"

/*!
   \class TimeStamp
   \brief time manipulation object
*/

class TimeStamp:public Counter 
{

  public:
  
  //! empty creator
  TimeStamp( void ):
    Counter( "TimeStamp" ),
    is_valid_( false ),
    time_( -1 )
  {}
      
  //! time_t creator
  TimeStamp( time_t time ):
    Counter( "TimeStamp" )
  { setTime( time ); }
  
  //! set time
  virtual bool setTime( time_t time )
  {
    is_valid_ = time >= 0;
    time_ = time;
    if( is_valid_ ) tm_ = *localtime( &time );
    return is_valid_;
  }
  
  //! convert tm_ into time_
  virtual bool makeTime( void )
  { 
    is_valid_ = (time_ = mktime( &tm_ ) ) >= 0;
    return is_valid_;
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
    
  //! used to have fast access to the IntOpt integer value
  virtual operator int (void) const 
  { return int( time_ ); }  

  //! default string for invalid timestamps
  static const std::string STAMP_UNKNOWN;
  
  //! timestamp format enumeration
  enum Format {
    
    //! DD/MM/YYYY
    DATE,
    
    //! YYYY/MM/DD
    DATE_US,
    
    //! HH:MM
    TIME,
    
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
  
  //! if timestamp is valid, returns formated string 
  virtual std::string string( Format format = SHORT ) const;

  //! returns time in second
  virtual time_t unixTime( void ) const 
  { return time_; }

  //! returns TimeStamp corresponding to _now_
  static TimeStamp now( void );
  
  //! retrieve seconds (between 0 and 59)
  virtual int seconds( void ) const
  { return (is_valid_) ? tm_.tm_sec:0; }
  
  //! retrieve minutes (between 0 and 59)
  virtual int minutes( void ) const
  { return (is_valid_) ? tm_.tm_min:0; }
  
  //! retrieve hour (between 0 and 23)
  virtual int hours( void ) const
  { return (is_valid_) ? tm_.tm_hour:0; }
  
  //! retrieves day (between 1 and 31)
  virtual int day( void ) const 
  { return (is_valid_) ? tm_.tm_mday:0; }       
  
  //! retrieves month (between 1 and 12)
  virtual int month( void ) const 
  { return (is_valid_) ? tm_.tm_mon+1:0; }      
  
  //! retrieves year (between 1 and 12)
  virtual int year( void ) const 
  { return (is_valid_) ? tm_.tm_year+1900:0; }  

  //! seconds (between 0 and 59)
  virtual void setSeconds( const int& value )
  { tm_.tm_sec = value; }
  
  //! minutes (between 0 and 59)
  virtual void setMinutes( const int& value )
  { tm_.tm_min = value; }
  
  //! hour (between 0 and 23)
  virtual void setHours( const int& value )
  { tm_.tm_hour = value; }
  
  //! retrieves day (between 1 and 31)
  virtual void setDay( const int& value ) 
  { tm_.tm_mday = value; }       
  
  //! retrieves month (between 1 and 12)
  virtual void setMonth( const int& value ) 
  { tm_.tm_mon = value - 1; }      
  
  //! retrieves year (between 1 and 12)
  virtual void setYear( const int& value ) 
  { tm_.tm_year = value - 1900; }  

  //! tells if timestamp is valid or not
  virtual void setValid( bool value ) 
  { is_valid_ = value; }
  
  //! checks if timestamp is valid or not
  virtual bool isValid( void ) const 
  { return is_valid_; }
  
  private:
      
  //! true if timeStamp is properly set
  bool is_valid_; 
  
  //! unix time in second 
  time_t time_;   
  
  //! time structure from localtime()
  struct tm tm_;  
  
  //! day names
  static const std::string days_[];    
  
  //! month names
  static const std::string months_[];  

};

#endif
