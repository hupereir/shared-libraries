#ifndef ValidFileThread_h
#define ValidFileThread_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA 02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
  \file ValidFileThread.h
  \brief check validity of a set of files
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QEvent>
#include <QThread>

#include "Counter.h"
#include "Debug.h" 
#include "FileRecord.h"

//! used to post a new grid when ready
class ValidFileEvent: public QEvent, public Counter
{
  
  public:
        
  //! constructor
  ValidFileEvent( const FileRecord::List& records, const bool& has_invalid_records ):
    QEvent( eventType() ),
    Counter( "ValidFileEvent" ),
    records_( records ),
    has_invalid_records_( has_invalid_records )
  {}
  
  //! destructor
  ~ValidFileEvent( void )
  {}
  
  //! static event type
  static QEvent::Type eventType( void );
  
  //! records
  const FileRecord::List& records()
  { return records_; }
  
  //! clean enabled
  const bool& hasInvalidRecords( void )
  { return has_invalid_records_; }
  
  private:
      
  //! ValidFile success flag
  FileRecord::List records_;
      
  //! true if some invalid files are present
  bool has_invalid_records_;
  
};

//! independent thread used to automatically save file
class ValidFileThread: public QThread, public Counter
{
    
  public:
  
  //! constructor
  ValidFileThread( QObject* reciever ):
    Counter( "ValidFileThread" ),
    reciever_( reciever ),
    check_duplicates_( true )
  {}

  //! check duplicates
  void setCheckDuplicates( bool value )
  { check_duplicates_ = value; }
  
  //! set file
  void setRecords( const FileRecord::List& records )
  { records_ = records; }
  
  //! Check files validity. Post a ValidFileEvent when finished
  void run( void );
  
  private:
 
  const bool& _checkDuplicates( void ) const
  { return check_duplicates_; }
  
  //! reciever object for posted events
  QObject* reciever_;
  
  //! check duplicates
  bool check_duplicates_;
  
  //! list of records to be checked
  FileRecord::List records_;
  
};
#endif
