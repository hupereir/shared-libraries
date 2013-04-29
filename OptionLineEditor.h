#ifndef OptionLineEditor_h
#define OptionLineEditor_h

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

#include "AnimatedLineEditor.h"
#include "OptionWidget.h"
#include "XmlOptions.h"

//! QLineEdit associated to an option for configuration dialogs
class OptionLineEditor: public AnimatedLineEditor, public OptionWidget
{

    public:

    //! constructor
    OptionLineEditor( QWidget* parent, const QString& optionName ):
        AnimatedLineEditor( parent ),
        OptionWidget( optionName )
    { _setBuddy( this ); }

    //! read value from option
    void read( void )
    { setText( XmlOptions::get().raw( optionName() ) ); }

    //! write value to option
    void write( void ) const
    { XmlOptions::get().setRaw( optionName(), text() ); }

};
#endif
