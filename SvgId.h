#ifndef SvgId_h
#define SvgId_h

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

#include <QSize>
#include <QString>
#include <QList>

//! svg namespace
namespace SVG
{
    class SvgId
    {
        public:

        typedef QList<SvgId> List;

        //! constructor
        SvgId( QSize size ):
            size_( size )
        {}

        //! constructor
        SvgId( QString name, QSize size ):
            id_( name ),
            size_( size )
        {}

        //! equal to operator
        bool operator == (const SvgId& id ) const
        { return size_ == id.size_ && id_ == id.id_; }

        //! less than operator
        bool operator < (const SvgId& id ) const
        {

            if( size_.width() != id.size_.width() ) return size_.width() < id.size_.width();
            else if( size_.height() != id.size_.height() ) return size_.height() < id.size_.height();
            else return id_ < id.id_;

        }

        //! id
        const QString& id( void ) const
        { return id_; }

        //! size
        const QSize& size( void ) const
        { return size_; }

        private:

        //! name
        QString id_;

        //! size
        QSize size_;

    };
};

#endif
