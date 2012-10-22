#ifndef OptionFontInfo_h
#define OptionFontInfo_h

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "OptionWidget.h"
#include "Options.h"
#include "FontInfo.h"
#include "TextFormat.h"

//! font formating options
class OptionFontInfo: public FontInfo, public OptionWidget
{

    public:

    //! constructor
    OptionFontInfo( QWidget* parent, const QString& optionName ):
        FontInfo( parent ),
        OptionWidget( optionName )
    { _setBuddy( this ); }

    //! read value from option
    void read( void )
    { setFormat( (FORMAT::TextFormatFlags) XmlOptions::get().get<unsigned int>( optionName() ) ); }

    //! write value to option
    void write( void ) const
    { XmlOptions::get().set<unsigned int>( optionName(), format() ); }

};
#endif
