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

#include "BaseReplaceDialog.h"
#include "Debug.h"

#include <QLabel>
#include <QPushButton>
#include <QApplication>

//________________________________________________________________________
QOrderedSet<QString>& BaseReplaceDialog::_replacedStrings()
{
    static QOrderedSet<QString> strings;
    return strings;
}

//________________________________________________________________________
BaseReplaceDialog::BaseReplaceDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseFindDialog( parent, flags )
{
    Debug::Throw( "BaseReplaceDialog::BaseReplaceDialog.\n" );

    // set dialog title
    setWindowTitle( tr( "Replace" ) );
    setOptionName( "REPLACE_DIALOG" );

    // create aditional widgets
    // insert text editor
    QLabel* label = new QLabel( tr( "Replace with:" ), &baseFindWidget() );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    baseFindWidget().editorLayout().addWidget( label, 1, 0, 1, 1 );

    // replacement editor
    baseFindWidget().editorLayout().addWidget( replaceEditor_ = new CustomComboBox( &baseFindWidget() ), 1, 1, 1, 1 );
    label->setBuddy( &_replaceEditor() );

    // disable callbacks on find editor
    disconnect( editor().lineEdit(), SIGNAL(textChanged(QString)), &baseFindWidget(), SLOT(findNoIncrement()) );

    replaceEditor_->setEditable( true );
    replaceEditor_->setAutoCompletion( true, Qt::CaseSensitive );
    connect( replaceEditor_->lineEdit(), SIGNAL(returnPressed()), SLOT(_replace()) );
    connect( replaceEditor_->lineEdit(), SIGNAL(returnPressed()), &baseFindWidget(), SLOT(updateFindComboBox()) );
    connect( replaceEditor_->lineEdit(), SIGNAL(returnPressed()), SLOT(_updateReplaceComboBox()) );
    connect( replaceEditor_->lineEdit(), SIGNAL(textChanged(QString)), SLOT(_replaceTextChanged(QString)) );

    // change tab order
    setTabOrder( &editor(), &_replaceEditor() );

    // location buttons
    QPushButton *button( 0 );
    QPushButton *previous( 0 );

    // insert selection button
    baseFindWidget().locationLayout().addWidget( new QLabel( tr( "Replace in:" ), &baseFindWidget() ) );
    baseFindWidget().locationLayout().addWidget( button = new QPushButton( tr( "Selection" ), &baseFindWidget() ) );
    button->setAutoDefault( false );
    connect( button, SIGNAL(clicked()), SLOT(_replaceInSelection()) );
    connect( button, SIGNAL(clicked()), &baseFindWidget(), SLOT(updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), SLOT(_updateReplaceComboBox()) );
    button->setToolTip( tr( "Replace all occurence of the search string in selected text" ) );
    baseFindWidget().addDisabledButton( button );
    button->setAutoDefault( false );

    // change tab order
    setTabOrder( &baseFindWidget().entireWordCheckBox(), button );
    previous = button;

    // insert window button
    baseFindWidget().locationLayout().addWidget( button = new QPushButton( tr( "Window" ), &baseFindWidget() ) );
    button->setAutoDefault( false );
    connect( button, SIGNAL(clicked()), SLOT(_replaceInWindow()) );
    connect( button, SIGNAL(clicked()), &baseFindWidget(), SLOT(updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), SLOT(_updateReplaceComboBox()) );
    button->setToolTip( tr( "Replace all occurence of the search string in the entire window" ) );
    baseFindWidget().addDisabledButton( button );
    button->setAutoDefault( false );
    replaceWindowButton_ = button;

    setTabOrder( previous, button );

    // replace buttons
    button = new QPushButton( tr( "Replace" ), this );
    connect( button, SIGNAL(clicked()), SLOT(_replace()) );
    connect( button, SIGNAL(clicked()), &baseFindWidget(), SLOT(updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), SLOT(_updateReplaceComboBox()) );
    baseFindWidget().addDisabledButton( button );
    _buttonLayout().insertWidget( 1, button );
    button->setAutoDefault( false );
    replaceButton_ = button;

    setTabOrder( &_findButton(), button );

}

//_____________________________________________________
BaseReplaceDialog::~BaseReplaceDialog( void )
{ Debug::Throw( "BaseReplaceDialog::~BaseReplaceDialog.\n" ); }

//_____________________________________________________
TextSelection BaseReplaceDialog::selection( const bool& no_increment ) const
{

    TextSelection out( BaseFindDialog::selection( no_increment ) );
    out.setReplaceText( replaceEditor_->currentText() );
    return out;

}

//__________________________________________________
void BaseReplaceDialog::synchronize( void )
{
    Debug::Throw( "BaseReplaceDialog::synchronize.\n" );

    // base class method
    BaseFindDialog::synchronize();

    // replace editor
    replaceEditor_->clear();
    for( QOrderedSet<QString>::iterator iter = _replacedStrings().begin(); iter != _replacedStrings().end(); ++iter )
    { replaceEditor_->addItem( *iter ); }

    // clear replace combobox text
    replaceEditor_->setEditText("");

    // set focus to find editor
    editor().setFocus();

}

//__________________________________________________
void BaseReplaceDialog::_replaceInWindow( void )
{ emit replaceInWindow( selection( false ) ); }

//__________________________________________________
void BaseReplaceDialog::_replaceInSelection( void )
{ emit replaceInSelection( selection( false ) ); }
