#ifndef FileRecord_h
#define FileRecord_h

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
#include "Functors.h"
#include "Debug.h"
#include "File.h"
#include "QOrderedSet.h"
#include "TimeStamp.h"

#include <QList>
#include <QHash>
#include <QStringList>

//* handles previously opened file and tags
class FileRecord: private Base::Counter<FileRecord>
{

    public:

    //* shortcut to list of records
    using Set = QOrderedSet<FileRecord>;
    using List = QList<FileRecord>;
    using ListIterator = QListIterator<FileRecord>;

    //* mimetype for drag and drop operations
    static const QString MimeType;

    //* constructor
    explicit FileRecord( const File& file = File(), const TimeStamp& time = TimeStamp::now() ):
        Counter( "FileRecord" ),
        file_( file ),
        time_( time ),
        valid_( true )
    {}

    //* constructor
    explicit FileRecord( File&& file, const TimeStamp& time = TimeStamp::now() ):
        Counter( "FileRecord" ),
        file_( std::move(file) ),
        time_( time ),
        valid_( true )
    {}

    //* destructor
    virtual ~FileRecord() = default;

    //*@name accessors
    //@{

    //* file
    const File& file() const { return file_; }

    //* time stamp
    const TimeStamp& time() const
    { return time_; }

    //* flags
    int flags() const
    { return flags_; }

    //* flags
    bool hasFlag( int flag ) const
    { return flags_ & flag; }

    //* validity
    bool isValid() const
    { return valid_; }

    //* map string to property ID
    class PropertyId
    {

        public:

        using Id = quint32;

        //* get id matching name
        /** insert in map if name is new */
        static Id get( QString );

        //* get name matching id
        /** throw exception if not found */
        static QString get( Id );

        private:

        //* counter
        static Id& _counter();

        //* id map
        using IdMap = QHash< QString, Id >;

        //* id map
        static IdMap& _idMap();

        //* id map
        using NameMap = QStringList;

        //* name map
        static NameMap& _nameMap();

    };

    //* true if property is available
    bool hasProperty( QString tag ) const
    { return hasProperty( PropertyId::get( tag ) ); }

    //* true if property is available
    bool hasProperty( PropertyId::Id id ) const
    { return properties_.find( id ) != properties_.end(); }

    //* additional property map
    using PropertyMap = QHash< PropertyId::Id, QString >;

    //* property map
    const PropertyMap& properties() const
    { return properties_; }

    //* retrieve property
    QString property( QString tag ) const
    { return property( PropertyId::get( tag ) ); }

    //* retrieve property
    QString property( PropertyId::Id id ) const
    {
        PropertyMap::const_iterator iter(  properties_.find( id ) );
        return ( iter == properties_.end() ) ? "":iter.value();
    }

    //@}

    //*@name modifiers
    //@{

    //* file
    FileRecord& setFile( const File& file )
    {
        file_ = file;
        return *this;
    }

    //* time stamp
    FileRecord& setTime( const TimeStamp& time )
    {
        time_ = time;
        return *this;
    }

    //* flags
    FileRecord& setFlags( int value )
    {
        flags_ = value;
        return *this;
    }

    //* flags
    FileRecord& setFlag( int flag, bool value = true )
    {
        if( value ) { flags_ |= flag; }
        else { flags_ &= (~flag); }
        return *this;
    }

    //* validity
    FileRecord& setValid( bool valid )
    {
        valid_ = valid;
        return *this;
    }

    //* add property
    FileRecord& addProperty( QString tag, QString value )
    { return addProperty( PropertyId::get( tag ), value ); }

    //* add property
    FileRecord& addProperty( PropertyId::Id, QString );

    //@}

    //* used to retrieve file records that match a given flag
    class HasFlagFTor
    {

        public:

        //* constructor
        explicit HasFlagFTor( int flag ):
            flag_( flag )
         {}

        //* predicate
        bool operator() ( const FileRecord& record ) const
        { return record.hasFlag( flag_ ); }

        private:

        // predicted flag
        int flag_;

    };

    //* used to sort records according to files
    using FileFTor = Base::Functor::BinaryLess<FileRecord, const File&, &FileRecord::file>;

    //* used to retrieve FileRecord with identical filenames
    using SameFileFTorUnary = Base::Functor::Unary<FileRecord, const File&, &FileRecord::file>;
    using SameFileFTorBinary = Base::Functor::Binary<FileRecord, const File&, &FileRecord::file>;

    //* used to sort FileRecords using canonical filenames
    class CanonicalFileFTor
    {

        public:

        //* predicate
        bool operator() (const FileRecord& first, const FileRecord& second) const
        { return first.file().canonicalName() < second.file().canonicalName(); }

    };

    //* used to remove FileRecord with identical canonical filenames
    class SameCanonicalFileFTor
    {

        public:

        //* constructor
        explicit SameCanonicalFileFTor( const File& file = File("") ):
            file_( file.canonicalName() )
        {}

        //* constructor
        explicit SameCanonicalFileFTor( const FileRecord& record ):
            file_( record.file().canonicalName() )
        {}

        //* predicate
        bool operator() (const FileRecord& first, const FileRecord& second) const
        { return first.file().canonicalName() == second.file().canonicalName(); }

        //* predicate
        bool operator() (const FileRecord& record ) const
        { return record.file().canonicalName() == file_; }

        private:

        //* filename
        File file_;

    };

    //* used to remove non-existing files
    class InvalidFTor
    {
        public:

        //* predicate
        bool operator()( const FileRecord& record )
        { return !record.isValid(); }

    };

    //* used to remove non-existing files
    class ValidFTor
    {
        public:

        //* predicate
        bool operator()( const FileRecord& record )
        { return record.isValid(); }

    };

    //* used to retrieve most recent file records
    using FirstOpenFTor = Base::Functor::BinaryLess<FileRecord, const TimeStamp&, &FileRecord::time>;

    private:

    //* file
    File file_;

    //* additional properties
    PropertyMap properties_;

    //* time
    TimeStamp time_;

    //* flags
    int flags_ = 0;

    //* file validity (true if file exists)
    bool valid_ = false;

    //* streamers
    friend QTextStream& operator << ( QTextStream& out, const FileRecord& record )
    {
        out << record.file() << endl;
        for( PropertyMap::const_iterator iter = record.properties().begin(); iter != record.properties().end(); iter++ )
        { out << "  " << iter.key() << " (" << PropertyId::get( iter.key() ) << "): " << iter.value() << endl; }
        return out;
    }

    //* less than operator
    friend bool operator < (const FileRecord&, const FileRecord& );

    //* less than operator
    friend bool operator == (const FileRecord&, const FileRecord& );

};

#endif
