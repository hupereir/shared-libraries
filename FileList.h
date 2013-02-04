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

#include "Counter.h"
#include "Debug.h"
#include "File.h"
#include "FileRecord.h"
#include "TimeStamp.h"
#include "ValidFileThread.h"

#include <QtCore/QObject>

//! handles list of files saved into resource file for later reopening
class FileList: public QObject, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    FileList( QObject* parent );

    //! destructor
    virtual ~FileList( void )
    {}

    //! returns true if file is found in list
    virtual bool contains( const File& file ) const;

    //! remove file from database
    virtual void remove( const File& file );

    //! get filerecord associated to a name
    /*! creates new fileRecord if not found */
    virtual FileRecord& get( const File& file )
    { return _add( FileRecord( file ), false ); }

    //! gets file list size
    virtual int size( void ) const
    { return _records().size(); }

    //! all records
    FileRecord::List records( void ) const
    { return _truncatedList( _records() ); }

    //! set record
    virtual void set( const FileRecord::List& );

    //! all files
    File::List files( void ) const;

    //! get last valid file
    virtual FileRecord lastValidFile( void );

    //! returns true if file list can be cleaned
    virtual bool cleanEnabled( void ) const
    { return (check()) ? cleanEnabled_ : !_records().empty(); }

    //! clean files. Remove either invalid or all files, depending on check_
    virtual void clean( void );

    //! clear files. Remove all
    virtual void clear( void );

    //! check flag
    virtual const bool& check( void ) const
    { return check_; }

    //! check_ flag
    virtual void setCheck( const bool& value )
    { check_ = value; }

    signals:

    //! emmited when thread has completed validity check
    void validFilesChecked( void );

    //! emmited when contents is changed
    void contentsChanged( void );

    public slots:

    //! add file.
    virtual FileRecord& add( const File& file )
    { return _add( FileRecord( file ) ); }

    //! run thread to check file validity
    void checkValidFiles( void );

    protected:

    //! maximum Size
    virtual void _setMaxSize( const int& value );

    //! maximum size
    virtual const int& _maxSize( void ) const
    { return maxSize_; }

    //! add record to current list
    virtual FileRecord& _add(
        const FileRecord&,
        const bool& = true,
        const bool& = true );

    //! truncate list if larger than maxSize_
    virtual FileRecord::List _truncatedList( FileRecord::List ) const;

    //! list of files records
    virtual const FileRecord::List& _records( void ) const
    { return records_; }

    //! list of files records
    virtual FileRecord::List& _records( void )
    { return records_; }

    protected slots:

    //! process records from threads
    void _processRecords( const FileRecord::List&, bool );

    private:

    //! clean enabled
    void _setCleanEnabled( const bool& value )
    { cleanEnabled_ = value; }

    //! maximum size (negative means no limit)
    int maxSize_;

    //! if true, check file validity
    bool check_;

    //! true if clean action is enabled
    bool cleanEnabled_;

    //! thread to check file validity
    ValidFileThread thread_;

    //! current list of files
    FileRecord::List records_;

};
#endif
