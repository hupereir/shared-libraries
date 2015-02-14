#ifndef BaseReplaceWidget_h
#define BaseReplaceWidget_h

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

#include "BaseFindWidget.h"

//* find dialog for text editor widgets
class BaseReplaceWidget: public BaseFindWidget
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseReplaceWidget( QWidget* = nullptr );

    //* destructor
    virtual ~BaseReplaceWidget( void ) = default;

    //*@name accessors
    //@{

    //* retrieve text selection from dialog
    virtual TextSelection selection( const bool& no_increment ) const;

    //* replace editor
    virtual CustomComboBox& replaceEditor( void ) const
    { return *replaceEditor_; }

    //* replace window button
    QAbstractButton& replaceWindowButton( void ) const
    { return *replaceWindowButton_; }

    //@}

    //*@name modifiers
    //@{

    //* string to replace
    void setReplaceText( const QString& text )
    { replaceEditor_->setEditText( text ); }

    //* string to replace
    QString replaceText( void ) const
    { return replaceEditor_->currentText(); }

    //* synchronize searched strings and ComboBox
    virtual void synchronize( void );

    //* add string to both combo box and static set
    void addReplacedString( const QString& );

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

    public Q_SLOTS:

    //* update combo box with current text
    void updateReplaceComboBox( void )
    { addReplacedString( replaceEditor_->currentText() ); }

    //* create Selection object when replace button is pressed
    void replace( void )
    { emit replace( selection( false ) ); }

    //* create Selection object when replace button is pressed
    void replaceInWindow( void );

    //* create Selection object when replace button is pressed
    void replaceInSelection( void );

    private:

    //* line editor for text to replace
    CustomComboBox* replaceEditor_;

    //* replace button
    QAbstractButton* replaceButton_;

    //* replace in window button
    QAbstractButton* replaceWindowButton_;

    //* replaced strings
    static QOrderedSet<QString>& _replacedStrings();

};
#endif
