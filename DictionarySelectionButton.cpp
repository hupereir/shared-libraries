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

#include "DictionarySelectionButton.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "SpellItemDialog.h"

namespace SPELLCHECK
{

    //________________________________________________________________
    DictionarySelectionButton::DictionarySelectionButton( QWidget* parent ):
        QToolButton( parent ),
        OptionWidget( "SPELLCHECK_DISABLED_DICTIONARIES" )
    {

        // make sure disabled dictionaries option exists
        if( !XmlOptions::get().contains( optionName() ) )
        { XmlOptions::get().set( optionName(), Option( " " ), true ); }

        // setup button
        setIcon( IconEngine::get( ICONS::CONFIGURE ) );
        setAutoRaise( true );
        setToolTip( "Configure dictionaries that should appear in the list" );
        connect( this, SIGNAL( clicked( void ) ), SLOT( _edit( void ) ) );

    }

    //___________________________________________
    void DictionarySelectionButton::_edit( void )
    {

        Debug::Throw( "DictionarySelectionButton::_edit.\n" );

        // value
        const QString backup( value_ );

        // create dialog
        SpellItemDialog dialog( this );
        dialog.setItems( SpellInterface().dictionaries() );
        dialog.setDisabledItems( backup );

        if( dialog.centerOnWidget( this ).exec() )
        { value_ = dialog.disabledItems(); }

        // check modification
        if( value_ != backup )
        {
            write();
            emit modified();
        }

        return;

    }

}
