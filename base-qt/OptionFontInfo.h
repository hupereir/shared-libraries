#ifndef OptionFontInfo_h
#define OptionFontInfo_h

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
#include "FontInfo.h"
#include "TextFormat.h"

//! font formating options
class OptionFontInfo: public FontInfo, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    OptionFontInfo( QWidget* parent, const QString& optionName ):
        FontInfo( parent ),
        OptionWidget( optionName, this )
    {}

    //! read value from option
    void read( const Options& options )
    { setFormat( (Format::TextFormatFlags) options.get<unsigned int>( optionName() ) ); }

    //! write value to option
    void write( Options& options ) const
    { options.set<unsigned int>( optionName(), format() ); }

};
#endif