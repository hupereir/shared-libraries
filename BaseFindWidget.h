#ifndef BaseFindWidget_h
#define BaseFindWidget_h

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
#include "CustomComboBox.h"
#include "TextSelection.h"
#include "QOrderedSet.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QList>

//* find dialog for text editor widgets
class BaseFindWidget: public QWidget, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseFindWidget( QWidget* = nullptr );

    //* destructor
    virtual ~BaseFindWidget( void ) = default;

    //*@name accessors
    //@{

    //* string to find
    virtual QString text( void ) const
    { return editor_->currentText(); }

    //* get selection
    virtual TextSelection selection( bool ) const;

    //* retrieve editor
    virtual CustomComboBox& editor( void ) const
    { return *editor_; }

    //* edtion layout
    QGridLayout& editorLayout( void ) const
    { return *editorLayout_; }

    //* locations layout
    QBoxLayout& locationLayout( void ) const
    { return *locationLayout_; }

    //* locations layout
    QBoxLayout& buttonLayout( void ) const
    { return *buttonLayout_; }

    //* "entire word" checkbox
    QCheckBox& entireWordCheckBox( void ) const
    { return *entireWordCheckbox_; }

    //* find button
    QAbstractButton& findButton( void ) const
    { return *findButton_; }

    //* close button
    QAbstractButton& closeButton( void ) const
    { return *closeButton_; }

    //* list of disabled buttons
    virtual QList<QAbstractButton*>& disabledButtons( void )
    { return buttons_; }

    //@}

    //*@name modifiers
    //@{

    //* string to find
    virtual void setText( const QString& );

    //* synchronize searched strings and ComboBox
    virtual void synchronize( void );

    //* enable/disable entire word
    virtual void enableEntireWord( bool );

    //* enable/disable RegExp
    virtual void enableRegExp( bool );

    //* add button to disabled button list
    virtual void addDisabledButton( QAbstractButton* );

    //* add string to both combo box and static set
    virtual void addSearchedString( const QString& );

    //@}

    Q_SIGNALS:

    //* emmited when Find is pressed
    void find( TextSelection );

    public Q_SLOTS:

    //* update combo box with current text
    virtual void updateFindComboBox( void )
    { addSearchedString( editor_->currentText() ); }

    //* create Selection object when find button is pressed
    virtual void find( void )
    { emit find( selection( false ) ); }

    //* take action when at least one match is found
    virtual void matchFound( void );

    //* take action when no match is found
    virtual void noMatchFound( void );

    //* create Selection object when find button is pressed
    virtual void findNoIncrement( void )
    { if( !regexpCheckbox_->isChecked() ) emit find( selection( true ) ); }

    //* update button state when regexp checkbox is checked
    virtual void regExpChecked( bool );

    //* update button state depending on the string to find
    virtual void updateButtons( const QString& text = QString() );

    private:

    //* editor layout
    QGridLayout* editorLayout_ = nullptr;

    //* location layout
    QBoxLayout* locationLayout_ = nullptr;

    //* button layout
    QBoxLayout* buttonLayout_ = nullptr;

    //* line editor for text to find
    CustomComboBox* editor_ = nullptr;

    //* backward search if checked
    QCheckBox* backwardCheckbox_ = nullptr;

    //* case sensitive search if checked
    QCheckBox* caseSensitiveCheckbox_ = nullptr;

    //* entire word check box
    QCheckBox* entireWordCheckbox_ = nullptr;

    //* regular expression search if checked
    QCheckBox* regexpCheckbox_ = nullptr;

    //* notification label
    QLabel* label_ = nullptr;

    //* find button
    QAbstractButton* findButton_ = nullptr;

    //* close button
    QAbstractButton* closeButton_ = nullptr;

    //* list of buttons to enable/disable depending of the editor text
    QList<QAbstractButton*> buttons_;

    //* set of previously searched strings
    static QOrderedSet<QString>& _searchedStrings( void );

};
#endif
