
#ifndef Debug_h
#define Debug_h

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
   \file    Debug.h
   \brief   Thread-safe option driven debugging class
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <iostream>

#include <QTextStream>
#include <QIODevice>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <stack>

/*!
   \class   Debug
   \brief   Thread-safe debugging class
*/

class Debug
{
  public:
    
  class LockedStream
  {
    
    public:
    
    //! constructor
    LockedStream( QTextStream& stream, QMutex& mutex ):
      mutex_lock_( &mutex ),
      stream_( stream )
    {}
     
    //! constructor
    LockedStream( const LockedStream& ref ):
      mutex_lock_( ref.mutex_lock_.mutex() ),
      stream_( ref.stream_ )
    {}
     
    //! destructor
    virtual ~LockedStream( void )
    {}
   
    //! cast to QTextStream
    operator QTextStream& () const 
    { return stream_; }

    //! stream operator
    template<typename T> LockedStream& operator<<(const T &r) 
    { 
      stream_ << r;
      return *this;
    }
    
    private:
    
    //! locker
    QMutexLocker mutex_lock_;
        
    //! stream
    QTextStream& stream_;
        
  };
  
  //! writes string to clog if level is lower than level_
  static void Throw( int level, QString str )
  {  
    // lock
    QMutexLocker lock( &_get()._mutex() );
    
    // check level
    if( _get().level_ < level ) return; 
    
    // print string to stream
    _get()._stdStream() << str << flush;
    
    return;
    
  }
  
  //! writes string to clog if level_ is bigger than 0
  static void Throw( QString str ) 
  { Throw( 1, str ); }
  
  //! returns either clog or dummy stream depending of the level
  static LockedStream Throw( int level = 1 )
  { 
    // return proper stream depending on debugging level
    return ( _get().level_ < level ) ? 
      LockedStream( _get()._nullStream(), _get()._mutex() ) : 
      LockedStream( _get()._stdStream(), _get()._mutex() ); 
  }
   
  //! sets the debug level. Everything thrown of bigger level is not discarded
  static void setLevel( const int& level ) 
  { _get().level_ = level; }
  
  //! retrieves the debug level
  static const int& level( void ) 
  { return _get().level_; }

  private:

  //! private constructor
  Debug( void ):
    level_( 0 ),
    null_stream_( &null_device_ ),
    std_stream_( stdout )
  {}
  
  //! return singleton
  static Debug& _get( void );
  
  //! mutex
  QMutex& _mutex( void )
  { return mutex_; }
  
  //! null device. 
  /*! Used to throw everything if the level is not high enough */  
  class NullIODevice : public QIODevice
  {
    public:
    
    //! constructor
    NullIODevice()
    {}
  
    protected:
    
    // read
    virtual qint64 readData ( char*, qint64 )
    { return 0; }
    
    // read
    virtual qint64 writeData( const char*, qint64 )
    { return 0; }
    
  };

  //! null device
  NullIODevice& _nullDevice( void )
  { return null_device_; }
  
  //! null device
  QTextStream& _nullStream( void )
  { return null_stream_; }
  
  //! null device
  QTextStream& _stdStream( void )
  { return std_stream_; }

  //! debug level
  int level_;  
  
  //! mutex
  mutable QMutex mutex_;
    
  //! null device
  NullIODevice null_device_; 

  //! null stream
  QTextStream null_stream_;
  
  //! default stream
  QTextStream std_stream_;

};

//Debug::LockedStream& endl( Debug::LockedStream& );

#endif
