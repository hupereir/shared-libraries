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

#include <QThread>
#include <QEvent>

#include "Counter.h"
#include "Debug.h" 
#include "FileRecord.h"

//! used to post a new grid when ready
class ValidFileEvent: public QEvent, public Counter
{
  
  public:
        
  //! constructor
  ValidFileEvent( const FileRecord::List& files ):
    QEvent( QEvent::User ),
    Counter( "ValidFileEvent" ),
    files_( files )
  {}
  
  //! destructor
  ~ValidFileEvent( void )
  {}
  
  //! grid
  const FileRecord::List& files()
  { return files_; }
  
  private:
      
  //! ValidFile success flag
  FileRecord::List files_;
      
};

//! independent thread used to automatically save file
class ValidFileThread: public QThread, public Counter
{
    
  public:
  
  //! constructor
  ValidFileThread( QObject* reciever ):
    Counter( "ValidFileThread" ),
    reciever_( reciever )
  {}

  
  //! set file
  void setFiles( const FileRecord::List& files )
  { files_ = files; }
  
  //! Check files validity. Post a ValidFileEvent when finished
  void run( void );
  
  private:
 
  //! reciever object for posted events
  QObject* reciever_;
  
  //! filename where data is to be saved
  FileRecord::List files_;
  
};
#endif
