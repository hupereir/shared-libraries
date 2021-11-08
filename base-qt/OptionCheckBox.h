#ifndef OptionCheckBox_h
#define OptionCheckBox_h

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
#include "base_qt_export.h"

#include <QCheckBox>

//! QCheckBox associated to an option for configuration dialogs
class BASE_QT_EXPORT OptionCheckBox: public QCheckBox, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    explicit OptionCheckBox( const QString& label, QWidget* parent, const QString& optionName ):
        QCheckBox( label, parent ),
        OptionWidget( optionName ),
        negative_( false )
    {}

    //! negative
    void setNegative( bool value )
    { negative_ = value; }

    //! read value from option
    void read( const Options& options ) override
    {
        if( negative_ ) setChecked( !options.get<bool>( optionName() ) );
        else setChecked( options.get<bool>( optionName() ) );
        if( !_connected() )
        {
            connect( this, &QAbstractButton::toggled, this, &OptionCheckBox::modified );
            _setConnected();
        }

    }

    //! write value to option
    void write( Options& options ) const override
    {
        if( negative_ ) options.set<bool>( optionName(), !isChecked() );
        else options.set<bool>( optionName(), isChecked() );
    }

    Q_SIGNALS:

    void modified();

    private:

    //! negative
    bool negative_;

};
#endif
