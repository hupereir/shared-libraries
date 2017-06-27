#ifndef BaseFindWidget_h
#define BaseFindWidget_h

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
#include "EmbeddedWidget.h"
#include "TextSelection.h"
#include "QOrderedSet.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QPalette>

//* find dialog for text editor widgets
class BaseFindWidget: public EmbeddedWidget, private Base::Counter<BaseFindWidget>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit BaseFindWidget( QWidget* = nullptr, bool compact = true );

    //*@name accessors
    //@{

    //* string to find
    virtual QString text() const
    { return editor_->currentText(); }

    //* get selection
    virtual TextSelection selection( bool ) const;

    //* retrieve editor
    CustomComboBox& editor() const
    { return *editor_; }

    //* close button
    QAbstractButton& closeButton() const
    { return *closeButton_; }

    //* list of disabled buttons
    QList<QAbstractButton*>& disabledButtons()
    { return buttons_; }

    //@}

    //*@name modifiers
    //@{

    //* string to find
    virtual void setText( const QString& );

    //* enable/disable entire word
    virtual void enableEntireWord( bool );

    //* enable/disable RegExp
    virtual void enableRegExp( bool );

    //* synchronize searched strings and ComboBox
    virtual void synchronize();

    //@}

    Q_SIGNALS:

    //* emitted when Find is pressed
    void find( TextSelection );

    public Q_SLOTS:

    //* take action when at least one match is found
    void matchFound();

    //* take action when no match is found
    void noMatchFound();

    protected Q_SLOTS:

    //* update combo box with current text
    void _updateFindComboBox()
    { _addSearchedString( editor_->currentText() ); }

    //* create Selection object when find button is pressed
    void _find()
    { emit find( selection( false ) ); }

    //* create Selection object when find button is pressed
    void _findPrevious()
    {
        findBackward_ = true;
        emit find( selection( false ) );
    }

    //* create Selection object when find button is pressed
    void _findNext()
    {
        findBackward_ = false;
        emit find( selection( false ) );
    }

    //* create Selection object when find button is pressed
    void _findNoIncrement()
    { if( !regexpCheckbox_->isChecked() ) emit find( selection( true ) ); }

    //* update button state when regexp checkbox is checked
    void _regExpChecked( bool );

    //* update button state depending on the string to find
    void _updateButtons( const QString& text = QString() );

    protected:

    //* change event
    void changeEvent( QEvent* ) override;

    //* "entire word" checkbox
    QCheckBox& _entireWordCheckBox() const
    { return *entireWordCheckbox_; }

    //* edition layout
    QGridLayout& _editorLayout() const
    { return *editorLayout_; }

    //* find next button
    QAbstractButton& _findNextButton() const
    { return *findNextButton_; }

    //* find previous button
    QAbstractButton& _findPreviousButton() const
    { return *findPreviousButton_; }

    //* add button to disabled button list
    void _addDisabledButton( QAbstractButton* );

    //* add string to both combo box and static set
    void _addSearchedString( const QString& );

    private:

    //* create not found palette
    void _updateNotFoundPalette();

    //* editor layout
    QGridLayout* editorLayout_ = nullptr;

    //* line editor for text to find
    CustomComboBox* editor_ = nullptr;

    //* backward search if checked
    // QCheckBox* backwardCheckbox_ = nullptr;
    bool findBackward_ = false;

    //* case sensitive search if checked
    QCheckBox* caseSensitiveCheckbox_ = nullptr;

    //* entire word check box
    QCheckBox* entireWordCheckbox_ = nullptr;

    //* regular expression search if checked
    QCheckBox* regexpCheckbox_ = nullptr;

    //* find button
    QAbstractButton* findNextButton_ = nullptr;
    QAbstractButton* findPreviousButton_ = nullptr;

    //* close button
    QAbstractButton* closeButton_ = nullptr;

    //* list of buttons to enable/disable depending of the editor text
    QList<QAbstractButton*> buttons_;

    //* not found palette
    QPalette notFoundPalette_;

    //* set of previously searched strings
    static QOrderedSet<QString>& _searchedStrings();

};
#endif
