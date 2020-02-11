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

#include "base_qt_export.h"
#include "Counter.h"
#include "Debug.h"
#include "FileRecord.h"
#include "ListModel.h"

#include <QHash>
#include <QMimeData>
#include <QStringList>

//* qlistview for object counters
class BASE_QT_EXPORT FileRecordModel: public ListModel<FileRecord>, private Base::Counter<FileRecordModel>
{

    Q_OBJECT

    public:

    //* column type enumeration
    enum ColumnType
    {
        FileName,
        Path,
        Time
    };

    //* constructor
    explicit FileRecordModel( QObject* = nullptr );

    //*@name accessors
    //@{

    //* show icons
    bool showIcons() const
    { return showIcons_; }

    //* flags
    Qt::ItemFlags flags( const QModelIndex& ) const override;

    // return data for a given index
    QVariant data( const QModelIndex &, int ) const override;

    //* header data
    QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const override;

    //* number of columns for a given index
    int columnCount( const QModelIndex& = QModelIndex() ) const override;

    //* column matching given name, if any
    int findColumn( const QString& ) const;

    //* mime data
    QMimeData* mimeData(const QModelIndexList& ) const override;

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
    void update( const List &values )
    {
        _updateColumns( values );
        ListModel<FileRecord>::update( values );
    }

    //@}

    protected:

    //* sort
    void _sort( int, Qt::SortOrder ) override;

    //* add, without update
    void _add( const ValueType& ) override;

    private:

    //* configuration
    void _updateConfiguration();

    //* used to sort Counters
    class BASE_QT_EXPORT SortFTor: public ItemModel::SortFTor
    {

        public:

        //* constructor
        explicit SortFTor( int type, Qt::SortOrder order, const QStringList& columnTitles ):
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
    static IconCache& _icons();

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
