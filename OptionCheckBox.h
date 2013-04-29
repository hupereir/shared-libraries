#ifndef OptionCheckBox_h
#define OptionCheckBox_h

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

#include <QCheckBox>

//! QCheckBox associated to an option for configuration dialogs
class OptionCheckBox: public QCheckBox, public OptionWidget
{

    public:

    //! constructor
    OptionCheckBox( const QString& label, QWidget* parent, const QString& optionName ):
        QCheckBox( label, parent ),
        OptionWidget( optionName ),
        negative_( false )
    { _setBuddy( this ); }

    //! negative
    void setNegative( bool value )
    { negative_ = value; }

    //! read value from option
    virtual void read( void )
    {
        if( negative_ ) setChecked( !XmlOptions::get().get<bool>( optionName() ) );
        else setChecked( XmlOptions::get().get<bool>( optionName() ) );
    }

    //! write value to option
    virtual void write( void ) const
    {
        if( negative_ ) XmlOptions::get().set<bool>( optionName(), !isChecked() );
        else XmlOptions::get().set<bool>( optionName(), isChecked() );
    }

    private:

    //! negative
    bool negative_;

};
#endif
