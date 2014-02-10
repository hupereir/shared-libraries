#ifndef HelpModel_h
#define HelpModel_h

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

/*!
\file HelpModel.h
\brief model for help items
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <string.h>


#include "ListModel.h"
#include "HelpItem.h"
#include "Debug.h"

namespace Base
{

    //! model for help items
    class HelpModel: public ListModel<HelpItem>, public Counter
    {

        //! Qt meta object declaration
        Q_OBJECT

        public:

        //! number of columns
        enum { nColumns = 1 };

        //! column type enumeration
        enum ColumnType { LABEL };

        //! constructor
        HelpModel( void ):
            Counter( "HelpModel" )
        {}

        //!@name methods reimplemented from base class
        //@{

        //! flags
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;


        // return data for a given index
        virtual QVariant data(const QModelIndex &index, int role) const;

        //! header data
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
        { return QVariant(); }

        //! number of columns for a given index
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
        { return nColumns; }

        //@}

        protected:

        //! sorting
        virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
        {}

    };

}

#endif
