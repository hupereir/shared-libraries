#ifndef SvgId_h
#define SvgId_h

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

#include "base_svg_export.h"
#include <QSize>
#include <QString>
#include <QList>

//* svg namespace
namespace Svg
{
    class BASE_SVG_EXPORT SvgId
    {
        public:

        using List = QList<SvgId>;

        //* constructor
        explicit SvgId( QSize size ):
            size_( size )
        {}

        //* constructor
        explicit SvgId( const QString &name, QSize size ):
            id_( name ),
            size_( size )
        {}

        //* id
        const QString& id() const
        { return id_; }

        //* size
        const QSize& size() const
        { return size_; }

        private:

        //* name
        QString id_;

        //* size
        QSize size_;

    };

    //* equal to operator
    inline bool operator == (const SvgId& first, const SvgId& second)
    { return first.size() == second.size() && first.id() == second.id(); }

    //* less than operator
    inline bool operator < (const SvgId& first, const SvgId& second)
    {

        if( first.size().width() != second.size().width() ) return first.size().width() < second.size().width();
        else if( first.size().height() != second.size().height() ) return first.size().height() < second.size().height();
        else return first.id() < second.id();

    }

};

#endif
