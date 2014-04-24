#ifndef XcbUtil_h
#define XcbUtil_h

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

#include "X11Defines.h"

#include <QHash>
#include <QWidget>

class XcbUtil: public X11Defines
{

    public:

    //! singleton
    static XcbUtil& get( void );

    //! destructor
    virtual ~XcbUtil( void );

    //! supported atoms
    bool isSupported( Atoms );

    //! return true if property is found in NET_WM_STATE
    bool hasProperty( QWidget*, Atoms );

//     //! change property
//     bool changeProperty( QWidget*, Atoms, bool);
//
//     //! change property
//     bool changeProperty( QWidget*, Atoms, const unsigned char*, int size );

    //! connection
    Qt::HANDLE connection( void ) const;

    //! atom
    Qt::HANDLE findAtom( Atoms ) const;

    //! application root window
    WId appRootWindow( void ) const;

    private:

    //! constructor
    XcbUtil( void );

    //! atom names
    typedef QMap<Atoms, QString> AtomNameMap;

    //! supported atoms
    typedef QHash<Atoms, bool > SupportedAtomMap;

    SupportedAtomMap supportedAtoms_;

    //! private pointer
    class Private;
    Private* d;

};

#endif
