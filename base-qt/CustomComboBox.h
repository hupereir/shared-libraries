#ifndef CustomComboBox_h
#define CustomComboBox_h

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

#include "Counter.h"
#include "Debug.h"
#include "LineEditor.h"

#include <QComboBox>
#include <QKeyEvent>

//* Customized QCombobox to handle case sensitive auto completion
class CustomComboBox: public QComboBox, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    CustomComboBox( QWidget* = nullptr );

    //* editability
    void setEditable( bool value );

    //* auto completion
    void setAutoCompletion( bool value, Qt::CaseSensitivity  = Qt::CaseInsensitive);

    //* set navigation enabled
    void setNavigationEnabled( bool value )
    { navigationEnabled_ = value; }

    protected:

    //* key press event
    virtual void keyPressEvent( QKeyEvent* );

    private:

    //* local line editor
    LineEditor* editor_ = nullptr;

    //* navigation enabled
    bool navigationEnabled_ = true;

};

#endif
