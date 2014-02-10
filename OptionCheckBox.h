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

#include <QCheckBox>

//! QCheckBox associated to an option for configuration dialogs
class OptionCheckBox: public QCheckBox, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    OptionCheckBox( const QString& label, QWidget* parent, const QString& optionName ):
        QCheckBox( label, parent ),
        OptionWidget( optionName, this ),
        negative_( false )
    {}

    //! negative
    void setNegative( bool value )
    { negative_ = value; }

    //! read value from option
    virtual void read( const Options& options )
    {
        if( negative_ ) setChecked( !options.get<bool>( optionName() ) );
        else setChecked( options.get<bool>( optionName() ) );
        if( !_connected() )
        {
            connect( this, SIGNAL(toggled(bool)), SIGNAL(modified()) );
            _setConnected();
        }

    }

    //! write value to option
    virtual void write( Options& options ) const
    {
        if( negative_ ) options.set<bool>( optionName(), !isChecked() );
        else options.set<bool>( optionName(), isChecked() );
    }

    Q_SIGNALS:

    void modified( void );

    private:

    //! negative
    bool negative_;

};
#endif
