#ifndef OptionRadioButton_h
#define OptionRadioButton_h

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

#include "OptionWidget.h"
#include "XmlOptions.h"

#include <QRadioButton>

//! QRadioButton associated to an option for configuration dialogs
class OptionRadioButton: public QRadioButton, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    OptionRadioButton( const QString& label, QWidget* parent, const QString& optionName ):
        QRadioButton( label, parent ),
        OptionWidget( optionName, this )
    { connect( this, SIGNAL(toggled(bool)), SIGNAL(modified())); }

    //! read value from option
    void read( void )
    { setChecked( XmlOptions::get().get<bool>( optionName() ) ); }

    //! write value to option
    void write( void ) const
    { XmlOptions::get().set<bool>( optionName(), isChecked() ); }

    Q_SIGNALS:

    //! modified
    void modified( void );

};
#endif
