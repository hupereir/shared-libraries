#ifndef TreeItemBase_h
#define TreeItemBase_h

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

//! used to wrap object T into tree structure
class TreeItemBase: public Counter
{

    public:

    //! unique id
    typedef unsigned int Id;

    //! unique id
    const Id& id( void ) const
    { return id_; }

    protected:

    //! constructor
    /*! used to insert T in the tree structure */
    TreeItemBase( const Id& id ):
        Counter( "TreeItemBase" ),
        id_( id )
    {}

    //! id
    void _setId( const Id& id )
    { id_ = id; }

    //! running id
    static Id& _runningId( void );

    private:

    //! unique id
    Id id_;

};

#endif
