#ifndef FileRecordModel_h
#define FileRecordModel_h

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
#include "Debug.h"
#include "FileRecord.h"
#include "ListModel.h"

#include <QHash>
#include <QMimeData>
#include <QStringList>

//* qlistview for object counters
class FileRecordModel: public ListModel<FileRecord>, public Counter
{

    Q_OBJECT

    public:

    //* column type enumeration
    enum ColumnType
    {
        Filename,
        Path,
        Time
    };

    //* constructor
    FileRecordModel( QObject* = nullptr );

    //*@name accessors
    //@{

    //* show icons
    virtual bool showIcons( void ) const
    { return showIcons_; }

    //* flags
    virtual Qt::ItemFlags flags( const QModelIndex& ) const;

    // return data for a given index
    virtual QVariant data( const QModelIndex &, int ) const;

    //* header data
    virtual QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const;

    //* number of columns for a given index
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const;

    //* column matching given name, if any
    virtual int findColumn( const QString& ) const;

    //* mime data
    virtual QMimeData* mimeData(const QModelIndexList& ) const;

    //@}

    //*@name modifiers
    //@{

    //* show icons
    void setShowIcons( bool value )
    { showIcons_ = value; }

    //* use local names
    void setUseLocalNames( bool value )
    { useLocalNames_ = value; }

    //* set values (overloaded)
    void set( const List& values )
    {
        _updateColumns( values );
        ListModel<FileRecord>::set( values );
    }

    //* set values (overloaded)
    void update( List values )
    {
        _updateColumns( values );
        ListModel<FileRecord>::update( values );
    }

    //@}

    protected:

    //* sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    //* add, without update
    virtual void _add( const ValueType& );

    private Q_SLOTS:

    //* configuration
    void _updateConfiguration( void );

    private:

    //* used to sort Counters
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //* constructor
        SortFTor( const int& type, Qt::SortOrder order, const QStringList& columnTitles ):
            ItemModel::SortFTor( type, order ),
            columnTitles_( columnTitles )
        {}

        //* prediction
        bool operator() ( FileRecord, FileRecord ) const;

        private:

        // column titles
        QStringList columnTitles_;

    };

    //* update columns
    void _updateColumns( const List& values )
    {

        // update columns
        for( const auto& record:values )
        { _updateColumns( record ); }

    }

    //* update columns
    void _updateColumns( const ValueType& value );

    //* icon
    static const QIcon& _icon( const QString& = QString() );

    //* icon cache
    using IconCache = QHash<QString, QIcon>;

    //* type icon cache
    static IconCache& _icons( void );

    //* local names
    bool useLocalNames_ = true;

    //* true if icons are to be shown
    bool showIcons_ = true;

    //* icon property id
    FileRecord::PropertyId::Id iconPropertyId_ = 0;

    //* column titles
    QStringList columnTitles_;

};

#endif
