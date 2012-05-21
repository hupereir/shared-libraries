#ifndef OptionComboBox_h
#define OptionComboBox_h

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

#include "CustomComboBox.h"
#include "OptionWidget.h"
#include "Options.h"
#include "XmlOptions.h"

//! QComboBox associated to an option for configuration dialogs
class OptionComboBox: public CustomComboBox, public OptionWidget
{

    public:

    //! constructor
    OptionComboBox( QWidget* parent, const QString& optionName ):
        CustomComboBox( parent ),
        OptionWidget( optionName )
    { _setBuddy( this ); }

    //! read value from option
    void read( void )
    {
        QString value( XmlOptions::get().raw( optionName() ) );
        int found( findText( value ) );
        if( found < 0 )
        {
            addItem( value );
            found = findText( value );
        }

        setCurrentIndex( found );
    }

    //! write value to option
    void write( void ) const
    { XmlOptions::get().setRaw( optionName(), currentText() ); }

};
#endif
