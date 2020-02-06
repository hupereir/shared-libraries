#ifndef FileList_h
#define FileList_h

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

#include "base_qt_export.h"
#include "Counter.h"
#include "Debug.h"
#include "File.h"
#include "FileRecord.h"
#include "TimeStamp.h"
#include "ValidFileThread.h"

#include <QObject>

//* handles list of files saved into resource file for later reopening
class BASE_QT_EXPORT FileList: public QObject, private Base::Counter<FileList>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit FileList( QObject* );

    //*@name accessors
    //@{

    //* returns true if file is found in list
    bool contains( const File& ) const;

    //* empty
    bool isEmpty() const
    { return records_.isEmpty(); }

    //* file list size
    int size() const
    { return records_.size(); }

    //* file list max size
    int maxSize() const
    { return maxSize_; }

    //* all records
    FileRecord::List records() const
    { return _truncatedList( records_ ); }

    //* all files
    File::List files() const;

    //* returns true if file list can be cleaned
    bool cleanEnabled() const
    { return (check()) ? cleanEnabled_ : !isEmpty(); }

    //* check flag
    bool check() const
    { return check_; }

    //@}

    //*@name modifiers
    //@{

    //* remove file from database
    void remove( const File& );

    //* get filerecord associated to a name
    /** creates new fileRecord if not found */
    FileRecord& get( const File& file )
    { return _add( FileRecord( file ), false ); }

    //* set record
    void set( const FileRecord::List& );

    //* get last valid file
    FileRecord lastValidFile();

    //* clean files. Remove either invalid or all files, depending on check_
    void clean();

    //* clear files. Remove all
    void clear();

    //* check_ flag
    void setCheck( bool value )
    { check_ = value; }

    //* maximum Size
    void setMaxSize( int );

    //* add file.
    FileRecord& add( const File& file )
    { return _add( FileRecord( file ) ); }

    //* run thread to check file validity
    void checkValidFiles();

    //@}

    Q_SIGNALS:

    //* emitted when thread has completed validity check
    void validFilesChecked();

    //* emitted when contents is changed
    void contentsChanged();

    protected:

    //* maximum size
    int _maxSize() const
    { return maxSize_; }

    //* add record to current list
    FileRecord& _add(
        const FileRecord&,
        bool = true,
        bool = true );

    //* truncate list if larger than maxSize_
    FileRecord::List _truncatedList( const FileRecord::List& ) const;

    //* list of files records
    const FileRecord::List& _records() const
    { return records_; }

    //* list of files records
    FileRecord::List& _records()
    { return records_; }

    private:

    //* process records from threads
    void _processRecords( const FileRecord::List&, bool );

    //* clean enabled
    void _setCleanEnabled( bool value )
    { cleanEnabled_ = value; }

    //* maximum size (zero means no limit)
    int maxSize_ = 0;

    //* if true, check file validity
    bool check_ = true;

    //* true if clean action is enabled
    bool cleanEnabled_ = false;

    //* thread to check file validity
    ValidFileThread thread_;

    //* current list of files
    FileRecord::List records_;

};
#endif
