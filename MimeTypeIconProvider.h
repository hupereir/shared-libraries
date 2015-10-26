#ifndef MimeTypeIconProvider_h
#define MimeTypeIconProvider_h

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

#include <QObject>
#include <QHash>
#include <QIcon>

//* FileInfo model. Stores file information for display in lists
class MimeTypeIconProvider : public QObject, public Counter
{

    public:

    //* constructor
    MimeTypeIconProvider( QObject* = nullptr );

    //* destructor
    virtual ~MimeTypeIconProvider() = default;

    //*@name accessors
    //@{

    //* icon matching given model index
    virtual const QIcon& icon( const QString& );

    //@}

    //*@name modifiers
    //@{

    //* insert item in map
    virtual void insert( const QString& key, const QString& value )
    { iconNames_.insert( key, value ); }

    //* clear
    virtual void clear( void )
    { icons_.clear(); }

    //@}

    private:

    //* icon names
    using IconNameCache = QHash<QString, QString>;
    IconNameCache iconNames_;

    //* icon cache
    using IconCache = QHash<QString, QIcon>;
    IconCache icons_;

    //* empty icon
    QIcon empty_;

};

#endif
