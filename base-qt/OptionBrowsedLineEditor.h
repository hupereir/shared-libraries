#ifndef OptionBrowsedLineEditor_h
#define OptionBrowsedLineEditor_h

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

#include "BrowsedLineEditor.h"
#include "LineEditor.h"
#include "OptionWidget.h"

//* QLineEdit associated to an option for configuration dialogs
class OptionBrowsedLineEditor: public BrowsedLineEditor, public OptionWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit OptionBrowsedLineEditor( QWidget* parent, const QString& optionName ):
        BrowsedLineEditor( parent ),
        OptionWidget( optionName, this )
    {}

    //*@name accessors
    //@{

    //* write value to option
    void write( Options& options ) const override
    { options.setRaw( optionName(), editor().text() ); }

    //@}

    //*@name modifiers
    //@{

    //* read value from option
    void read( const Options& options ) override
    {
        setFile( options.raw( optionName() ) );
        if( !_connected() )
        {
            connect( &editor(), SIGNAL(textChanged(QString)), SIGNAL(modified()) );
            _setConnected();
        }
    }

    //@}

    Q_SIGNALS:

    //* modified signal
    void modified();

};
#endif
