#ifndef OptionIconSizeComboBox_h
#define OptionIconSizeComboBox_h

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

#include "IconSizeComboBox.h"
#include "OptionWidget.h"
#include "base_qt_export.h"

//* QComboBox associated to an option for configuration dialogs
class BASE_QT_EXPORT OptionIconSizeComboBox: public IconSizeComboBox, public OptionWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit OptionIconSizeComboBox( QWidget* parent, const QString& optionName ):
        IconSizeComboBox( parent, true ),
        OptionWidget( optionName )
    {}

    //* read value from option
    void read( const Options& options ) override
    {
        IconSize::Size size = static_cast<IconSize::Size>( options.get<int>( optionName() ) );
        selectIconSize( size );

        if( !_connected() )
        {
            connect( this, QOverload<int>::of(&IconSizeComboBox::currentIndexChanged), this, [this](int){ emit modified(); } );
            _setConnected();
        }

    }

    //* write value to option
    void write( Options& options ) const override
    { options.set<int>( optionName(), iconSize() ); }

    Q_SIGNALS:

    //* modified
    void modified();

};
#endif
