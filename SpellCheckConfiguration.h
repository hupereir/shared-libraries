#ifndef SpellCheckConfiguration_h
#define SpellCheckConfiguration_h

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

#include "Counter.h"
#include "OptionWidgetList.h"

#include <QGroupBox>

class OptionComboBox;

namespace SpellCheck
{
    //* Spell common configuration
    class SpellCheckConfiguration: public QGroupBox, public OptionWidgetList, public Counter
    {

        Q_OBJECT

        public:

        enum Flag
        {
            None = 0,
            ASpell = 1<<0,
            Dictionary = 1<<1,
            Filter = 1<<2,
            All = ASpell | Dictionary | Filter
        };

        Q_DECLARE_FLAGS( Flags, Flag );

        //* constructor
        SpellCheckConfiguration( QWidget*, Flags = All );

        //* destructor
        virtual ~SpellCheckConfiguration( void ) = default;

        Q_SIGNALS:

        //* modified
        void modified( void );

        protected Q_SLOTS:

        //* update dictionaries
        void _updateDictionaries( void );

        //* update filters
        void _updateFilters( void );

        private:

        OptionComboBox* dictionariesComboBox_ = nullptr;
        OptionComboBox* filtersComboBox_ = nullptr;

    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS( SpellCheck::SpellCheckConfiguration::Flags );

#endif
