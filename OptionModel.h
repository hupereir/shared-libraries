#ifndef OptionModel_h
#define OptionModel_h

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
#include "Options.h"
#include "TreeModel.h"

class OptionPair: public Options::Pair
{

    public:

    //! constructor
    OptionPair( void )
    {}

    //! constructor
    OptionPair( const Options::Pair& option ):
        Options::Pair( option )
     {}

    //! constructor
    OptionPair( const QString& name, const Option& option ):
        Options::Pair( name, option )
    {}

    //! equal to operator
    bool operator == ( const OptionPair& other ) const
    { return first == other.first && second.raw() == other.second.raw(); }

    //! returns true if this record is a child of argument
    bool isChild( const OptionPair& other ) const
    { return other.first == first && other.second.raw().isEmpty(); }

    //! write to stream
    friend QTextStream& operator << (QTextStream& out, const OptionPair& pair )
    {
        out << pair.first << " " << pair.second.raw();
        return out;
    }

};

//! qlistview for object IconCaches
class OptionModel: public TreeModel<OptionPair>, public Counter
{

    Q_OBJECT

    public:

    //! column type enumeration
    enum ColumnType
    {
        NAME,
        CURRENT,
        VALUE,
        DEFAULT_VALUE,
        FLAGS
    };

    //! constructor
    OptionModel( QObject* parent = 0 ):
        TreeModel<OptionPair>( parent ),
        Counter( "OptionModel" ),
        readOnly_( true )
    {}

    //! destructor
    virtual ~OptionModel( void )
    {}

    //! set model read only
    void setReadOnly( const bool& value )
    { readOnly_ = value; }

    //! is read only
    const bool& isReadOnly( void ) const
    { return readOnly_; }

    //!@name methods reimplemented from base class
    //@{

    //! flags
    virtual Qt::ItemFlags flags( const QModelIndex& ) const;

    // return data for a given index
    virtual QVariant data( const QModelIndex&, int ) const;

    // modify data
    virtual bool setData( const QModelIndex&, const QVariant&, int = Qt::EditRole );

    //! header data
    virtual QVariant headerData( int, Qt::Orientation, int role = Qt::DisplayRole ) const;

    //! number of columns for a given index
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const
    { return nColumns; }

    //@}

    Q_SIGNALS:

    //! emmited when one special option is modified
    void specialOptionModified( OptionPair );

    //! emmited when one option is modified
    void optionModified( OptionPair );

    protected:

    //! sorting
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    { _root().sort( SortFTor( column, order ) ); }

    private:

    //! used to sort IconCaches
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //! constructor
        SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
            ItemModel::SortFTor( type, order )
        {}

        //! prediction
        bool operator() ( const Item& first, const Item& second ) const
        { return (*this)( first.get(), second.get() ); }

        //! prediction
        bool operator() ( OptionPair, OptionPair ) const;

    };

    //! true if read only
    bool readOnly_;

    //! number of columns
    enum { nColumns = 5 };

    //! column titles
    static const QString columnTitles_[ nColumns ];

};

#endif
