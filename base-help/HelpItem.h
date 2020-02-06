#ifndef HelpItem_h
#define HelpItem_h

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

#include "base_help_export.h"
#include "Counter.h"
#include "Debug.h"

#include <QList>

//* Help system namespace
namespace Base
{

    //* reference manual single entry
    class BASE_HELP_EXPORT HelpItem final: private Counter<HelpItem>
    {

        public:

        //* constructor
        explicit HelpItem( const QString& label = QString(), const QString& text = QString() ):
            Counter( QStringLiteral("HelpItem") ),
            label_( label ),
            text_( text )
        { Debug::Throw( QStringLiteral("HelpItem::HelpItem.\n") ); }

        //* item label
        void setLabel( const QString& label )
        { label_ = label; }

        //* label
        const QString& label() const
        { return label_; }

        //* item text
        void setText( const QString& text )
        { text_ = text; }

        //* item text
        const QString& text() const
        { return text_; }

        //* shortcut to help item list
        using List = QList< HelpItem >;

        private:

        //* help label
        QString label_;

        //* help text
        QString text_;

    };

    //* equal-to operator
    inline bool operator == (const HelpItem& first, const HelpItem& second)
    { return first.label() == second.label() && first.text() == second.text(); }

    //* less than operator
    inline bool operator < (const HelpItem& first, const HelpItem& second)
    { return first.label() < second.label() || (first.label() == second.label() && first.text() < second.text() ); }

}

#endif
