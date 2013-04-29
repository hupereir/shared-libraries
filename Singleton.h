#ifndef Singleton_h
#define Singleton_h

// $Id$

/***********************************************************************
*
* Copyright 2013 Hugo PEREIRA DA COSTA <hugo.pereira@free.fr>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#include <QObject>

//! a class singleton used to centralize all objects that need static creation
class Singleton
{

    public:

    //! return singleton
    static Singleton& get( void );

    //! destructor
    virtual ~Singleton( void )
    {}

    //!@name accessors
    //@{

    //! true if has application
    bool hasApplication( void ) const
    { return (bool) application_; }

    //! application
    QObject* application()
    {
        Q_CHECK_PTR( application_ );
        return application_;
    }

    //! cast
    template< typename T >
        T* application( void )
    {
        Q_CHECK_PTR( application_ );
        return static_cast<T*>( application_ );
    }

    //@}

    //!@name modifiers
    //@{

    //! set application
    void setApplication( QObject* application )
    {
        Q_ASSERT( !application_ );
        application_ = application;
    }

    //@}

    private:

    //! constructor
    Singleton( void ):
        application_(0)
    {}

    QObject* application_;

};

#endif
