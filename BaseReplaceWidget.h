#ifndef BaseReplaceWidget_h
#define BaseReplaceWidget_h

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

#include "BaseFindWidget.h"

#include <QMenu>

//* find dialog for text editor widgets
class BaseReplaceWidget: public BaseFindWidget
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseReplaceWidget( QWidget* = nullptr, bool compact = true );

    //*@name accessors
    //@{

    //* string to replace
    QString replaceText( void ) const
    { return replaceEditor_->currentText(); }

    //* retrieve text selection from dialog
    virtual TextSelection selection( bool ) const;

    //@}

    //*@name modifiers
    //@{

    //* string to replace
    void setReplaceText( const QString& text )
    { replaceEditor_->setEditText( text ); }

    //* synchronize searched strings and ComboBox
    virtual void synchronize( void );

    //@}

    Q_SIGNALS:

    //* emmited when Replace is pressed
    void replace( TextSelection );

    //* emmited when Replace_in_window is pressed
    void replaceInWindow( TextSelection );

    //* emmited when Replace_in_selection is pressed
    void replaceInSelection( TextSelection );

    //* emmited when text replacement is changed
    void replaceTextChanged( QString text_replace );

    protected Q_SLOTS:

    //* update combo box with current text
    void _updateReplaceComboBox( void )
    { _addReplacedString( replaceEditor_->currentText() ); }

    //* create Selection object when replace button is pressed
    void _replace( void )
    { emit replace( selection( false ) ); }

    //* create Selection object when replace button is pressed
    void _replaceInWindow( void );

    //* create Selection object when replace button is pressed
    void _replaceInSelection( void );

    protected:

    //* replace editor
    virtual CustomComboBox& _replaceEditor( void ) const
    { return *replaceEditor_; }

    //* replace all menu
    virtual QMenu& _replaceAllMenu( void ) const
    { return *replaceAllMenu_; }

    //* add string to both combo box and static set
    virtual void _addReplacedString( const QString& );

    private:

    //* location layout
    QBoxLayout* locationLayout_ = nullptr;

    //* line editor for text to replace
    CustomComboBox* replaceEditor_ = nullptr;

    //* replace all menu
    QMenu* replaceAllMenu_ = nullptr;

    //* replaced strings
    static QOrderedSet<QString>& _replacedStrings();

};
#endif
