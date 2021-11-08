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

#include "FontInfo.h"
#include "OptionWidget.h"
#include "TextFormat.h"
#include "base_qt_export.h"

//! font formating options
class BASE_QT_EXPORT OptionFontInfo: public FontInfo, public OptionWidget
{

    Q_OBJECT

    public:

    //! constructor
    explicit OptionFontInfo( QWidget* parent, const QString& optionName ):
        FontInfo( parent ),
        OptionWidget( optionName )
    {}

    //! read value from option
    void read( const Options& options ) override
    { setFormat( static_cast<TextFormat::Flags>( options.get<int>( optionName() ) ) ); }

    //! write value to option
    void write( Options& options ) const override
    { options.set<int>( optionName(), format() ); }

};
#endif
