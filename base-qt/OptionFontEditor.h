#ifndef OptionFontEditor_h
#define OptionFontEditor_h

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

#include "FontEditor.h"
#include "OptionWidget.h"
#include "base_qt_export.h"

//! QLineEdit associated to an option for configuration dialogs
class BASE_QT_EXPORT OptionFontEditor: public FontEditor, public OptionWidget
{

    Q_OBJECT

    public:

        //! constructor
    explicit OptionFontEditor( QWidget* parent, const QString& optionName ):
        FontEditor( parent ),
        OptionWidget( optionName )
    {}

    //! read value from option
    void read( const Options& options ) override
    {
        setFont( options.raw( optionName() ) );
        if( !_connected() )
        {
            connect( this, &FontEditor::fontChanged, this, &OptionFontEditor::modified);
            _setConnected();
        }

    }

    //! write value to option
    void write( Options& options ) const override
    { options.setRaw( optionName(), font().toString() ); }

    Q_SIGNALS:

    //! modified
    void modified();

};

#endif
