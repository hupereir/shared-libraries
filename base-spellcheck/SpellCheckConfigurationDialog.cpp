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

#include "SpellCheckConfigurationDialog.h"

#include "AutoSpellConfiguration.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "SpellCheckConfiguration.h"

namespace SpellCheck
{
    //_________________________________________________________
    ConfigurationDialog::ConfigurationDialog( QWidget* parent ):
        BaseConfigurationDialog( parent )
    {

        setWindowTitle( tr( "Spell Check Configuration" ) );

        // base configuration
        // TODO: move icon names to enum
        auto page = &addPage( IconEngine::get( "tools-check-spelling" ), tr( "Configuration" ), tr( "Aspell configuration" ) );
        auto spellCheckConfiguration = new SpellCheckConfiguration( page );
        page->layout()->addWidget( spellCheckConfiguration );
        addOptionWidget( spellCheckConfiguration );

        page = &addPage( IconEngine::get( IconNames::PreferencesAppearance ), tr( "Appearance" ), tr( "Automatic spell check appearance" ) );
        auto autoSpellConfiguration = new AutoSpellConfiguration( page );
        page->layout()->addWidget( autoSpellConfiguration );
        addOptionWidget( autoSpellConfiguration );

        // load initial configuration
        read();

    }

}
