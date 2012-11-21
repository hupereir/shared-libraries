#ifndef _FileRecordModel_h_
#define _FileRecordModel_h_

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

#include "Counter.h"
#include "Debug.h"
#include "FileRecord.h"
#include "ListModel.h"

#include <QtCore/QStringList>
#include <QtCore/QHash>

//! qlistview for object counters
class FileRecordModel: public ListModel<FileRecord>, public Counter
{

    Q_OBJECT

    public:

    //! column type enumeration
    enum ColumnType {
        FILE,
        PATH,
        TIME
    };

    //! constructor
    FileRecordModel( QObject* = 0 );

    //! destructor
    virtual ~FileRecordModel( void )
    {}

    //! show icons
    void setShowIcons( const bool& value )
    { showIcons_ = value; }

    //! use local names
    void setUseLocalNames( const bool& value )
    { useLocalNames_ = value; }

    //!@name methods reimplemented from base class
    //@{

    //! flags
    virtual Qt::ItemFlags flags( const QModelIndex& ) const;

    // return data for a given index
    virtual QVariant data( const QModelIndex &, int ) const;

    //! header data
    virtual QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const;

    //! number of columns for a given index
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const;

    //! column matching given name, if any
    virtual int findColumn( const QString& ) const;

    //@}

    //! set values (overloaded)
    void set( const List& values )
    {
        _updateColumns( values );
        ListModel<FileRecord>::set( values );
    }

    //! set values (overloaded)
    void update( const List& values )
    {
        _updateColumns( values );
        ListModel<FileRecord>::update( values );
    }

    //! enable drag
    const bool& dragEnabled( void ) const
    { return dragEnabled_; }

    //! enable drag
    void setDragEnabled( const bool& value )
    { dragEnabled_ = value; }

    protected:

    //! sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    //! add, without update
    virtual void _add( const ValueType& );

    private slots:

    //! configuration
    void _updateConfiguration( void );

    private:

    //! used to sort Counters
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //! constructor
        SortFTor( const int& type, Qt::SortOrder order, const QStringList& columnTitles ):
            ItemModel::SortFTor( type, order ),
            columnTitles_( columnTitles )
        {}

        //! prediction
        bool operator() ( FileRecord, FileRecord ) const;

        private:

        // column titles
        QStringList columnTitles_;

    };

    //! update columns
    void _updateColumns( const List& values )
    {

        // update columns
        foreach( const FileRecord& record, values )
        { _updateColumns( record ); }

    }

    //! update columns
    void _updateColumns( const ValueType& value );

    //! icon
    static const QIcon& _icon( const QString& = QString() );

    //! icon cache
    typedef QHash<QString, QIcon> IconCache;

    //! type icon cache
    static IconCache& _icons( void );

    //! drag flag
    bool dragEnabled_;

    //! local names
    bool useLocalNames_;

    //! true if icons are to be shown
    bool showIcons_;

    //! icon property id
    FileRecord::PropertyId::Id iconPropertyId_;

    //! column titles
    QStringList columnTitles_;

};

#endif
