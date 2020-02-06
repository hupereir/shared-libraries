#ifndef Singleton_h
#define Singleton_h

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

#include "base_export.h"
#include "Counter.h"

#include <QObject>

namespace Base{

    //* a class singleton used to centralize all objects that need static creation
    class BASE_EXPORT Singleton final: public QObject, private Counter<Singleton>
    {

        Q_OBJECT

        public:

        //* return singleton
        static Singleton& get();

        //*@name accessors
        //@{

        //* true if has application
        bool hasApplication() const
        { return (bool) application_; }

        //* application
        QObject* application()
        { return application_; }

        //* cast
        template< class T >
        T* application()
        { return static_cast<T*>( application_ ); }

        //@}

        //*@name modifiers
        //@{

        //* set application
        template< class T >
        void setApplication( T* application )
        {
            application_ = application;
            connect( application, &T::configurationChanged, this, &Singleton::configurationChanged );
            connect( this, &Singleton::requestConfigurationChanged, application, &T::configurationChanged );
        }

        //@}

        Q_SIGNALS:

        // request configuration changed
        void requestConfigurationChanged();

        // configuration changed
        void configurationChanged();

        private:

        //* constructor
        explicit Singleton():
            Counter( QStringLiteral("Singleton") )
        {}

        QObject* application_ = nullptr;

    };

}

#endif
