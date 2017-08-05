#ifndef OptionModel_h
#define OptionModel_h

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

#include <array>

//___________________________________________________________________
inline bool operator == ( const Options::Pair& first, const Options::Pair& second )
{ return first.first == second.first && first.second.raw() == second.second.raw(); }

//___________________________________________________________________
namespace Base
{
    //* isChild operator, needed for TreeModel
    template<> bool isChild<Options::Pair>( const Options::Pair&, const Options::Pair& );

}

//___________________________________________________________________
class OptionModel: public TreeModel<Options::Pair>, private Base::Counter<OptionModel>
{

    Q_OBJECT

    public:

    //* column type enumeration
    enum ColumnType
    {
        Name,
        Current,
        Value,
        DefaultValue,
        Flags,
        nColumns
    };

    //* constructor
    explicit OptionModel( QObject* parent = nullptr ):
        TreeModel<Options::Pair>( parent ),
        Counter( "OptionModel" )
    {}

    //* set model read only
    void setReadOnly( bool value )
    { readOnly_ = value; }

    //* is read only
    bool isReadOnly() const
    { return readOnly_; }

    //*@name methods reimplemented from base class
    //@{

    //* flags
    Qt::ItemFlags flags( const QModelIndex& ) const override;

    // return data for a given index
    QVariant data( const QModelIndex&, int ) const override;

    // modify data
    bool setData( const QModelIndex&, const QVariant&, int = Qt::EditRole ) override;

    //* header data
    QVariant headerData( int, Qt::Orientation, int role = Qt::DisplayRole ) const override;

    //* number of columns for a given index
    int columnCount( const QModelIndex& = QModelIndex() ) const override
    { return nColumns; }

    //@}

    Q_SIGNALS:

    //* emitted when one special option is modified
    void specialOptionModified( Options::Pair );

    //* emitted when one option is modified
    void optionModified( Options::Pair );

    protected:

    //* sorting
    void _sort( int column, Qt::SortOrder order ) override
    { _root().sort( SortFTor( column, order ) ); }

    private:

    //* used to sort IconCaches
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //* constructor
        explicit SortFTor( int type, Qt::SortOrder order ):
            ItemModel::SortFTor( type, order )
        {}

        //* prediction
        bool operator() ( const Item& first, const Item& second ) const
        { return (*this)( first.get(), second.get() ); }

        //* prediction
        bool operator() ( Options::Pair, Options::Pair ) const;

    };

    //* true if read only
    bool readOnly_ = true;

    //* column titles
    const std::array<QString, nColumns> columnTitles_ =
    {{
        tr( "Name" ),
        "",
        tr( "Current Value" ),
        tr( "Default Value" ),
        tr( "Flags" )
    }};

};

#endif
