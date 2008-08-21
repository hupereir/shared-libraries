#ifndef FileList_h
#define FileList_h

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
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
   \file FileList.h
   \brief handles list of files saved into resource file for later reopening
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <assert.h>
#include <list>     
#include <string>   
#include <QObject>   

#include "Counter.h"
#include "Debug.h"
#include "File.h"
#include "FileRecord.h"
#include "TimeStamp.h"
#include "ValidFileThread.h"

//! handles list of files saved into resource file for later reopening
class FileList: public QObject, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:

  //! constructor
  FileList( QObject* parent ):
    QObject( parent ),
    Counter( "FileList" ),
    max_size_( -1 ),
    check_( true ),
    thread_( this )
  {}

  //! destructor
  virtual ~FileList( void )
  { Debug::Throw( "FileList::~FileList.\n" ); }

  //! returns true if file is found in list
  virtual bool has( const File& file ) const;
  
  //! add file.
  virtual FileRecord& add( const File& file )
  { return _add( FileRecord( file ) ); }

  //! remove file from database
  virtual void remove( const File& file );

  //! get filerecord associated to a name
  /*! creates new fileRecord if not found */
  virtual FileRecord& get( const File& file )
  { return _add( FileRecord( file ), false ); }
   
  //! gets file list size
  virtual int size( void ) const
  { return records().size(); }
  
  //! get all files
  std::list< File > files( void ) const;
  
  //! get last valid file
  virtual FileRecord lastValidFile( void );
    
  //! returns true if file list can be cleaned
  virtual bool cleanEnabled( void ) const;
    
  //! clean files. Remove either invalid or all files, depending on check_
  virtual void clean( void );
  
  //! check flag
  virtual const bool& check( void ) const
  { return check_; }

  //! check_ flag
  virtual void setCheck( const bool& value )
  { check_ = value; }
  
   //! list of files records
  virtual const FileRecord::List& records( void ) const
  { return records_; }
 
  //! list of files records
  virtual FileRecord::List& records( void )
  { return records_; }

  signals:
    
  //! emmited when thread has completed validity check
  void validFilesChecked( void );
  
  //! emmited when contents is changed
  void contentsChanged( void );
  
  public slots:
    
  //! run thread to check file validity
  void checkValidFiles( void );
  
  protected:
  
  //! custom event, used to retrieve file validity check event
  void customEvent( QEvent* );
   
  //! maximum Size
  virtual void _setMaxSize( const int& value );

  //! maximum size
  virtual const int& _maxSize( void ) const
  { return max_size_; }
 
  //! add record to current list
  virtual FileRecord& _add( 
    const FileRecord& record, 
    const bool& update_timestamp = true,
    const bool& emit_signal = true );
  
  //! truncate list if larger than max_size_
  virtual void _truncateList( void );

  private:
  
  //! maximum size (negative means no limit)
  int max_size_;

  //! if true, check file validity
  bool check_;
  
  //! thread to check file validity
  ValidFileThread thread_;
  
  //! current list of files
  FileRecord::List records_;

};
#endif
