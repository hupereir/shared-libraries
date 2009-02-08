#ifndef _HelpModel_h_
#define _HelpModel_h_

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

namespace BASE
{

  //! model for help items
  class HelpModel: public ListModel<HelpItem>, public Counter
  {

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! used to tag Keyword drags
    static const QString DRAG;
    
    //! number of columns
    enum { n_columns = 1 };
    
    //! column type enumeration
    enum ColumnType { LABEL };
    
    //! constructor
    HelpModel( void ):
      Counter( "HelpModel" ),
      edition_enabled_( false )
      {}
    
    //!@name methods reimplemented from base class
    //@{
 
    //! flags
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    
    
    // return data for a given index
    virtual QVariant data(const QModelIndex &index, int role) const;
    
    // modify data
    virtual bool setData(const QModelIndex &index, const QVariant& value, int role = Qt::EditRole );
    
    //! header data
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    { return QVariant(); }
    
    //! number of columns for a given index
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    { return n_columns; }
    
    //! mime type
    virtual QStringList mimeTypes( void ) const;
    
    //! mime data
    virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
    
    //! drop mine data
    virtual bool dropMimeData(const QMimeData*, Qt::DropAction, int row, int column, const QModelIndex&);
    
    //@}
    
    //! edition
    void setEditionEnabled( const bool& value )
    { edition_enabled_ = value; }
    
    //! edition
    const bool& editionEnabled( void ) const
    { return edition_enabled_; }

    signals:
    
    //! emitted when an item is dragged to a different position
    void itemMoved( int row );

    //! emmited when an item label is renamed
    void itemRenamed( QModelIndex index, QString label );
    
    protected:
    
    //! sorting
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    {}
    
    private:
    
    //! edition enabled
    bool edition_enabled_;
    
  };

};

#endif
