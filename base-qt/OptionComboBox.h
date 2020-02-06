#ifndef OptionComboBox_h
#define OptionComboBox_h

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

#include "base_qt_export.h"
#include "CustomComboBox.h"
#include "OptionWidget.h"

//* QComboBox associated to an option for configuration dialogs
class BASE_QT_EXPORT OptionComboBox: public CustomComboBox, public OptionWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit OptionComboBox( QWidget* parent, const QString& optionName ):
        CustomComboBox( parent ),
        OptionWidget( optionName )
    {}

    //* use value for option
    void setUseValue( bool value )
    { useValue_ = value; }

    //* read value from option
    void read( const Options& options ) override
    {
        if( useValue_ )
        {
            const QString value( options.raw( optionName() ) );
            int found( findText( value ) );
            if( found < 0 )
            {
                addItem( value );
                found = findText( value );
            }

            setCurrentIndex( found );

        } else setCurrentIndex( options.get<int>( optionName() ) );

        if( !_connected() )
        {
            connect( this, &QComboBox::editTextChanged, this, &OptionComboBox::modified );
            connect( this, SIGNAL(currentIndexChanged(int)), SIGNAL(modified()) );
            _setConnected();
        }
    }

    //* write value to option
    void write( Options& options ) const override
    {
        if( useValue_ ) options.setRaw( optionName(), currentText() );
        else options.set<int>( optionName(), currentIndex() );
    }

    Q_SIGNALS:

    //* modified
    void modified();

    private:

    //* use value for option
    bool useValue_ = true;

};
#endif
