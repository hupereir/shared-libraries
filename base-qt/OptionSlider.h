#ifndef OptionSlider_h
#define OptionSlider_h

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
#include "Slider.h"
#include "base_qt_export.h"

#include <cmath>

//* QSlider associated to an option for configuration dialogs
class BASE_QT_EXPORT OptionSlider: public Slider, public OptionWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit OptionSlider( QWidget* parent, const QString& optionName ):
        Slider( parent ),
        OptionWidget( optionName )
    {}

    //* scale (i.e. option = value()/scale)
    void setScale( double scale )
    { scale_ = scale; }

    //* read value from option
    void read( const Options& options ) override
    {
        setValue( static_cast<int>(round(scale_*options.get<double>( optionName() ))));
        if( !_connected() )
        {
            connect( this, &Slider::valueChanged, this, &OptionSlider::modified);
            _setConnected();
        }
    }

    //* write value to option
    void write( Options& options ) const override
    { options.set<double>( optionName(), static_cast<double>(slider().value())/scale_ ); }

    Q_SIGNALS:

    //* modified
    void modified();

    private:

    //* scale factor (default is 1)
    double scale_ = 1;

};
#endif
