#ifndef BaseFindDialog_h
#define BaseFindDialog_h

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

#include "BaseDialog.h"
#include "Counter.h"
#include "CustomComboBox.h"
#include "TextSelection.h"
#include "QOrderedSet.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QList>

//! find_text dialog for text editor widgets
class BaseFindDialog: public BaseDialog, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    BaseFindDialog( QWidget* parent = 0, Qt::WindowFlags WindowFlags = 0 );

    //! retrieve editor
    virtual CustomComboBox& editor( void ) const
    { return *editor_; }

    //! string to find
    virtual void setText( const QString& text )
    {
        _addSearchedString( text );
        editor().setEditText( text );
        editor().lineEdit()->selectAll();
    }

    //! synchronize searched strings and ComboBox
    virtual void synchronize( void );

    //! string to find
    virtual QString text( void ) const
    { return editor().currentText(); }

    //! enable/disable entire word
    virtual void enableEntireWord( const bool& value )
    {
        if( !value ) entireWordCheckbox_->setChecked( false );
        entireWordCheckbox_->setEnabled( value );
    }

    //! enable/disable RegExp
    virtual void enableRegExp( const bool& value )
    {
        if( !value ) regexpCheckbox_->setChecked( false );
        regexpCheckbox_->setEnabled( value );
    }

    //! get selection
    virtual TextSelection selection( const bool& noIncrement ) const
    {

        TextSelection out( editor().currentText() );
        out.setFlag( TextSelection::Backward, backwardCheckbox_->isChecked() );
        out.setFlag( TextSelection::CaseSensitive, caseSensitiveCheckbox_->isChecked() );
        out.setFlag( TextSelection::EntireWord, entireWordCheckbox_->isChecked() );
        out.setFlag( TextSelection::RegExp, regexpCheckbox_->isChecked() );
        out.setFlag( TextSelection::NoIncrement, noIncrement );
        return out;
    }

    signals:

    //! emmited when Find is pressed
    void find( TextSelection );

    public slots:

    //! set notification label to "no match found"
    virtual void noMatchFound( void );

    //! clear notification label
    virtual void clearLabel( void );

    protected slots:

    //! update combo box with current text
    virtual void _updateFindComboBox( void )
    { _addSearchedString( editor().currentText() ); }

    //! create Selection object when find button is pressed
    virtual void _find( void )
    { emit find( selection( false ) ); }

    //! create Selection object when find button is pressed
    virtual void _findNoIncrement( void )
    { if( !regexpCheckbox_->isChecked() ) emit find( selection( true ) ); }

    //! update button state when regexp checkbox is checked
    virtual void _regExpChecked( bool );

    //! update button state depending on the string to find
    virtual void _updateButtons( const QString& text = QString() );

    protected:

    //! edtion layout
    QGridLayout& _editorLayout() const
    { return *editorLayout_; }

    //! locations layout
    QBoxLayout& _locationLayout() const
    { return *locationLayout_; }

    //! button layout
    QBoxLayout& _buttonLayout() const
    { return *buttonLayout_; }

    //! "entire word" checkbox
    QCheckBox& _entireWordCheckBox() const
    { return *entireWordCheckbox_; }

    //! "find" button
    QPushButton& _findButton( void ) const
    { return *findButton_; }

    //! add button to disabled button list
    virtual void _addDisabledButton( QAbstractButton* button )
    { buttons_ << button; }

    //! add string to both combo box and static set
    virtual void _addSearchedString( const QString& text  )
    {
        if( text.isEmpty() ) return;
        if( _searchedStrings().find( text ) == _searchedStrings().end() )
        {
            _searchedStrings().insert( text );
            editor().addItem( text );
        }
    }

    //! list of disabled buttons
    virtual QList<QAbstractButton*>& _disabledButtons( void )
    { return buttons_; }

    private:

    //! editor layout
    QGridLayout* editorLayout_;

    //! location layout
    QBoxLayout* locationLayout_;

    //! button layout
    QBoxLayout* buttonLayout_;

    //! line editor for text to find
    CustomComboBox* editor_;

    //! backward search if checked
    QCheckBox* backwardCheckbox_;

    //! case sensitive search if checked
    QCheckBox* caseSensitiveCheckbox_;

    //! entire word check box
    QCheckBox* entireWordCheckbox_;

    //! regular expression search if checked
    QCheckBox* regexpCheckbox_;

    //! find button
    QPushButton* findButton_;

    //! list of buttons to enable/disable depending of the editor text
    QList<QAbstractButton*> buttons_;

    //! set of previously searched strings
    static QOrderedSet<QString>& _searchedStrings( void );

    //! notification label
    QLabel* label_;

};
#endif
