#ifndef AbstractFindWidget_h
#define AbstractFindWidget_h

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

#include "EmbeddedWidget.h"
#include "TextSelection.h"

#include <QAbstractButton>
#include <QWidget>

//* abstract find widget class
class AbstractFindWidget: public EmbeddedWidget
{

    Q_OBJECT

    public:

    //* constructor
    explicit AbstractFindWidget( QWidget* parent = nullptr ):
    EmbeddedWidget( parent )
    {}

    //*@name accessors
    //@{

    //* string to find
    virtual QString text() const = 0;

    //* selection
    virtual TextSelection selection( bool ) const = 0;

    //* editor
    virtual QWidget& editor() const = 0;

    //* close button
    virtual QAbstractButton& closeButton() const = 0;

    //@}

    //*@name modifiers
    //@{

    //* string to find
    virtual void setText( const QString& )
    {}

    //* synchronize searched strings and ComboBox
    virtual void synchronize()
    {}

    //* take action when at least one match is found
    virtual void matchFound()
    {}

    //* take action when no match is found
    virtual void noMatchFound()
    {}

    //@}

    Q_SIGNALS:

    //* emitted when Find is pressed
    void find( TextSelection );

};

#endif
