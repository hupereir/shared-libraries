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

#include "FilterSelectionButton.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "SpellItemDialog.h"
#include "XmlOptions.h"

namespace SpellCheck
{

    //________________________________________________________________
    FilterSelectionButton::FilterSelectionButton( QWidget* parent ):
        QToolButton( parent ),
        OptionWidget( QStringLiteral("SPELLCHECK_DISABLED_FILTERS") )
    {

        // make sure disabled filters option exists
        if( !XmlOptions::get().contains( optionName() ) )
        { XmlOptions::get().set( optionName(), Option( " " ), true ); }

        // setup button
        setIcon( IconEngine::get( IconNames::Configure ) );
        setAutoRaise( true );
        setToolTip( QStringLiteral("Configure filters that should appear in the list") );
        connect( this, &QAbstractButton::clicked, this, &FilterSelectionButton::_edit );

        read( XmlOptions::get() );

    }

    //___________________________________________
    void FilterSelectionButton::_edit()
    {

        Debug::Throw( QStringLiteral("FilterSelectionButton::_edit.\n") );

        // value
        const QString backup( value_ );

        // create dialog
        SpellItemDialog dialog( this );
        dialog.setItems( SpellInterface().filters() );
        dialog.setDisabledItems( backup );

        if( dialog.centerOnWidget( this ).exec() )
        { value_ = dialog.disabledItems(); }

        // check modification
        if( value_ != backup )
        {
            write( XmlOptions::get() );
            if( _connected() ) emit modified();
        }

        return;

    }

}
