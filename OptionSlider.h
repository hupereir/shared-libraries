#ifndef OptionSlider_h
#define OptionSlider_h

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

#include "CustomSlider.h"
#include "OptionWidget.h"
#include "Options.h"

#include <cmath>

//! QSlider associated to an option for configuration dialogs
class OptionSlider: public CustomSlider, public OptionWidget
{

    public:

    //! constructor
    OptionSlider( QWidget* parent, const QString& optionName ):
        CustomSlider( parent ),
        OptionWidget( optionName ),
        scale_( 1 )
    { _setBuddy( this ); }

    //! scale (i.e. option = value()/scale)
    void setScale( const double& scale )
    { scale_ = scale; }

    //! read value from option
    void read( void )
    { setValue( static_cast<int>(round(scale_*XmlOptions::get().get<double>( optionName() )))); }

    //! write value to option
    void write( void ) const
    { XmlOptions::get().set<double>( optionName(), static_cast<double>(slider().value())/scale_ ); }

    private:

    //! scale factor (default is 1)
    double scale_;

};
#endif
