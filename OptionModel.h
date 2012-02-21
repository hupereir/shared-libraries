#ifndef _OptionModel_h_
#define _OptionModel_h_

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

/*!
\file OptionModel.h
\brief model for options
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <string.h>


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

    //! returns true if this record is a child of argument
    bool isChild( const OptionPair& option ) const
    { return option.first == first && option.second.raw().isEmpty(); }

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

    //! number of columns
    enum { nColumns = 5 };

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

    //! set model read only
    void setReadOnly( const bool& value )
    { readOnly_ = value; }

    //! is read only
    const bool& isReadOnly( void ) const
    { return readOnly_; }

    //!@name methods reimplemented from base class
    //@{

    //! flags
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    // return data for a given index
    virtual QVariant data(const QModelIndex &index, int role) const;

    // modify data
    virtual bool setData(const QModelIndex &index, const QVariant& value, int role = Qt::EditRole );

    //! header data
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    //! number of columns for a given index
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    { return nColumns; }

    //@}

    signals:

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

    //! column titles
    static const QString columnTitles_[ nColumns ];

};

#endif
