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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
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
        OptionWidget( optionName ),
        useValue_( true )
    { _setBuddy( this ); }

    //! use value for option
    void setUseValue( bool value )
    { useValue_ = value; }

    //! read value from option
    void read( void )
    {
        if( useValue_ )
        {
            const QString value( XmlOptions::get().raw( optionName() ) );
            int found( findText( value ) );
            if( found < 0 )
            {
                addItem( value );
                found = findText( value );
            }

            setCurrentIndex( found );

        } else setCurrentIndex( XmlOptions::get().get<int>( optionName() ) );
    }

    //! write value to option
    void write( void ) const
    {
        if( useValue_ ) XmlOptions::get().setRaw( optionName(), currentText() );
        else XmlOptions::get().set<int>( optionName(), currentIndex() );
    }

    private:

    //! use value for option
    bool useValue_;

};
#endif
