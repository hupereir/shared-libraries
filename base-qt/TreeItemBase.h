#ifndef TreeItemBase_h
#define TreeItemBase_h

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

//* used to wrap object T into tree structure
class BASE_QT_EXPORT TreeItemBase: private Base::Counter<TreeItemBase>
{

    public:

    //* destructor
    virtual ~TreeItemBase() = default;

    //* unique id
    using Id = quint32;

    //* unique id
    const Id& id() const
    { return id_; }

    protected:

    //* constructor
    /** used to insert T in the tree structure */
    explicit TreeItemBase( TreeItemBase::Id id ):
        Counter( QStringLiteral("TreeItemBase") ),
        id_( id )
    {}

    //* id
    void _setId( TreeItemBase::Id id )
    { id_ = id; }

    //* running id
    static Id& _runningId();

    private:

    //* unique id
    Id id_;

};

#endif
