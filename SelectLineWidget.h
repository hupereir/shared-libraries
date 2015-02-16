#ifndef SelectLineWidget_h
#define SelectLineWidget_h

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

#include "Counter.h"
#include "EmbeddedWidget.h"

#include <QAbstractButton>
#include <QPalette>
#include <QWidget>

class LineEditor;

//* find_text dialog for text editor widgets
class SelectLineWidget: public EmbeddedWidget, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    SelectLineWidget( QWidget* = nullptr, bool compact = true );

    //* retrieve editor
    LineEditor& editor( void ) const
    { return *editor_; }

    //* ok button
    QAbstractButton& okButton( void ) const
    { return *okButton_; }

    //* close button
    QAbstractButton& closeButton( void ) const
    { return *closeButton_; }

    Q_SIGNALS:

    //* emmited when pressing the Ok button
    void lineSelected( int );

    public Q_SLOTS:

    //* take action when at least one match is found
    virtual void matchFound( void );

    //* take action when no match is found
    virtual void noMatchFound( void );

    protected:

    //* change event
    virtual void changeEvent( QEvent* );

    private Q_SLOTS:

    //* retrieve line number and emit signal
    void _selectLine( void );

    private:

    //* create not found palette
    void _updateNotFoundPalette( void );

    //* line editor for text to find
    LineEditor* editor_ = nullptr;

    //* ok button
    QAbstractButton* okButton_ = nullptr;

    //* cancel button
    QAbstractButton* closeButton_ = nullptr;

    //* not found palette
    QPalette notFoundPalette_;

};
#endif
