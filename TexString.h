#ifndef TexString_h
#define TexString_h

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

#include <QString>
#include <QList>
#include <QPair>
#include <QSet>

//! some tex related utilities
namespace SPELLCHECK
{

    class TexString: public QString
    {

        public:

        //! constructor
        TexString( const QString& value ):
            QString( value )
        {}

        //! convert to latex accents
        QString toLatexAccents( void ) const;

        //! convert to french accents
        QString toTextAccents( void ) const;

        //! conversions
        typedef QPair<QString, QString> Conversion;

        //! conversions
        typedef QList< Conversion > ConversionList;

        //! conversions
        static const ConversionList& conversions( void );

    };

}

#endif
