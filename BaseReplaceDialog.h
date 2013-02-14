#ifndef BaseReplaceDialog_h
#define BaseReplaceDialog_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "Counter.h"
#include "CustomComboBox.h"
#include "BaseFindDialog.h"

//! replace_text dialogs
class BaseReplaceDialog: public BaseFindDialog
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    BaseReplaceDialog( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    //! destructor
    virtual ~BaseReplaceDialog( void );

    //! string to replace
    void setReplaceText( const QString& text )
    { replaceEditor_->setEditText( text ); }

    //! string to replace
    QString replaceText( void ) const
    { return replaceEditor_->currentText(); }

    //! retrieve text selection from dialog
    virtual TextSelection selection( const bool& no_increment ) const;

    //! synchronize searched strings and ComboBox
    virtual void synchronize( void );

    signals:

    //! emmited when Replace is pressed
    void replace( TextSelection );

    //! emmited when Replace_in_window is pressed
    void replaceInWindow( TextSelection );

    //! emmited when Replace_in_selection is pressed
    void replaceInSelection( TextSelection );

    //! emmited when text replacement is changed
    void replaceTextChanged( QString text_replace );

    protected slots:

    //! update combo box with current text
    void _updateReplaceComboBox( void )
    { _addReplacedString( replaceEditor_->currentText() ); }

    //! create Selection object when replace button is pressed
    void _replace( void )
    { emit replace( selection( false ) ); }

    //! create Selection object when replace button is pressed
    void _replaceInWindow( void );

    //! create Selection object when replace button is pressed
    void _replaceInSelection( void );

    //! emmited when text replacement is changed
    void _replaceTextChanged( const QString& text )
    { emit replaceTextChanged( text ); }

    protected:

    //! replace editor
    virtual CustomComboBox& _replaceEditor( void )
    { return *replaceEditor_; }

    //! replace editor
    const virtual CustomComboBox& _replaceEditor( void ) const
    { return *replaceEditor_; }

    //! replace window button
    QPushButton& _replaceWindowButton( void ) const
    { return *replaceWindowButton_; }

    private:

    //! replaced strings
    static QOrderedSet<QString>& _replacedStrings();

    //! add string to both combo box and static set
    void _addReplacedString( const QString& text  )
    {
        if( text.isEmpty() ) return;
        if( _replacedStrings().find( text ) == _replacedStrings().end() )
        {
            _replacedStrings().insert( text );
            replaceEditor_->addItem( text );
        }
    }

    //! line editor for text to replace
    CustomComboBox* replaceEditor_;

    //! replace in window button
    QPushButton* replaceWindowButton_;

};
#endif
