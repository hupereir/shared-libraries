#ifndef Singleton_h
#define Singleton_h

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

#include <QtCore/QObject>

//! a class singleton used to centralize all objects that need static creation
class Singleton
{

    public:

    //! return singleton
    static Singleton& get( void );

    //! set application
    void setApplication( QObject* application )
    {
        Q_ASSERT( !application_ );
        application_ = application;
    }

    //! application
    QObject* application()
    {
        Q_ASSERT( application_ );
        return application_;
    }

    //! cast
    template< typename T >
        T* application( void )
    {
        Q_ASSERT( application_ );
        return static_cast<T*>( application_ );
    }

    //! destructor
    ~Singleton( void )
    {}

    private:

    //! constructor
    Singleton( void ):
        application_(0)
    {}

    QObject* application_;

};

#endif
