#ifndef TexString_h
#define TexString_h

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

#include "base_spellcheck_export.h"
#include "NonCopyable.h"

#include <QString>
#include <QList>
#include <QPair>
#include <QSet>

//* some tex related utilities
namespace SpellCheck
{

    class BASE_SPELLCHECK_EXPORT TexString final: private Base::NonCopyable<TexString>
    {

        public:

        //* universal constructor
        template<typename... Args>
        explicit TexString(Args&&... args):
            value_( std::forward<Args>(args)... )
        {}

        //* convert to latex accents
        QString toLatexAccents() const;

        //* convert to french accents
        QString toTextAccents() const;

        //* conversions
        using Conversion = QPair<QString, QString>;

        //* conversions
        using ConversionList = QList< Conversion >;

        //* conversions
        static const ConversionList& conversions();

        private:

        QString value_;

    };

}

#endif
