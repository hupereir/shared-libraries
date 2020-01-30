#ifndef OptionRadioButton_h
#define OptionRadioButton_h

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

#include <QRadioButton>

//! QRadioButton associated to an option for configuration dialogs
class OptionRadioButton: public QRadioButton, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    explicit OptionRadioButton( const QString& label, QWidget* parent, const QString& optionName ):
        QRadioButton( label, parent ),
        OptionWidget( optionName, this )
    {}

    //! read value from option
    void read( const Options& options ) override
    {
        setChecked( options.get<bool>( optionName() ) );
        if( !_connected() )
        {
            connect( this, &QAbstractButton::toggled, this, &OptionRadioButton::modified);
            _setConnected();
        }

    }

    //! write value to option
    void write( Options& options ) const override
    { options.set<bool>( optionName(), isChecked() ); }

    Q_SIGNALS:

    //! modified
    void modified();

};
#endif
