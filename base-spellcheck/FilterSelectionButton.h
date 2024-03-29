#ifndef FilterSelectionButton_h
#define FilterSelectionButton_h

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

#include "OptionWidget.h"
#include "base_spellcheck_export.h"

#include <QToolButton>

namespace SpellCheck
{

    class BASE_SPELLCHECK_EXPORT FilterSelectionButton: public QToolButton, public OptionWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit FilterSelectionButton( QWidget* );

        //* retrieve value
        QString value() const
        { return value_; }

        //* read value from option
        void read( const Options& options ) override
        {
            value_ = options.raw( optionName() );
            _setConnected();
        }

        //* write value to option
        void write( Options& options ) const override
        { options.set( optionName(), value_ ); }

        Q_SIGNALS:

        //* emitted when selection is modified
        void modified();

        private:

        //* edit filters
        void _edit();

        //* disabled filters
        QString value_;

    };

}

#endif
