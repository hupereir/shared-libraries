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

#include "BaseReplaceWidget.h"
#include "Debug.h"

#include <QPushButton>

//________________________________________________________________________
QOrderedSet<QString>& BaseReplaceWidget::_replacedStrings()
{
    static QOrderedSet<QString> strings;
    return strings;
}

//________________________________________________________________________
BaseReplaceWidget::BaseReplaceWidget( QWidget* parent ):
    BaseFindWidget( parent )
{
    Debug::Throw( "BaseReplaceWidget::BaseReplaceWidget.\n" );

    // create aditional widgets
    // insert text editor
    QLabel* label = new QLabel( tr( "Replace with:" ), this );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    _editorLayout().addWidget( label, 1, 0, 1, 1 );

    // replacement editor
    _editorLayout().addWidget( replaceEditor_ = new CustomComboBox( this ), 1, 1, 1, 1 );
    label->setBuddy( replaceEditor_ );

    // disable callbacks on find editor
    disconnect( editor().lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(_findNoIncrement()) );

    replaceEditor_->setEditable( true );
    replaceEditor_->setAutoCompletion( true, Qt::CaseSensitive );
    connect( replaceEditor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT(_replace()) );
    connect( replaceEditor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT(_updateFindComboBox()) );
    connect( replaceEditor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT(_updateReplaceComboBox()) );
    connect( replaceEditor_->lineEdit(), SIGNAL(textChanged(QString)), this, SIGNAL(replaceTextChanged(QString)) );

    // change tab order
    setTabOrder( &editor(), replaceEditor_ );

    // location buttons
    QPushButton *button( 0 );
    QPushButton *previous( 0 );

    // insert selection button
    _locationLayout().addWidget( new QLabel( tr( "Replace in:" ), this ) );
    _locationLayout().addWidget( button = new QPushButton( tr( "Selection" ), this ) );
    button->setAutoDefault( false );
    connect( button, SIGNAL(clicked()), this, SLOT(_replaceInSelection()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateReplaceComboBox()) );
    button->setToolTip( tr( "Replace all occurence of the search string in selected text" ) );
    _addDisabledButton( button );
    button->setAutoDefault( false );

    // change tab order
    setTabOrder( &_entireWordCheckBox(), button );
    previous = button;

    // insert window button
    _locationLayout().addWidget( button = new QPushButton( tr( "Window" ), this ) );
    button->setAutoDefault( false );
    connect( button, SIGNAL(clicked()), this, SLOT(_replaceInWindow()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateReplaceComboBox()) );
    button->setToolTip( tr( "Replace all occurence of the search string in the entire window" ) );
    _addDisabledButton( button );
    button->setAutoDefault( false );
    replaceWindowButton_ = button;

    setTabOrder( previous, button );

    // replace buttons
    button = new QPushButton( tr( "Replace" ), this );
    connect( button, SIGNAL(clicked()), this, SLOT(_replace()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateReplaceComboBox()) );
    _addDisabledButton( button );
    _buttonLayout().insertWidget( 1, button );
    button->setAutoDefault( false );
    replaceButton_ = button;

    setTabOrder( &findButton(), button );

}

//_____________________________________________________
TextSelection BaseReplaceWidget::selection( const bool& noIncrement ) const
{

    TextSelection out( BaseFindWidget::selection( noIncrement ) );
    out.setReplaceText( replaceEditor_->currentText() );
    return out;

}

//__________________________________________________
void BaseReplaceWidget::synchronize( void )
{
    Debug::Throw( "BaseReplaceWidget::synchronize.\n" );

    // base class method
    BaseFindWidget::synchronize();

    // replace editor
    replaceEditor_->clear();
    foreach( const auto& string, _replacedStrings() )
    { replaceEditor_->addItem( string ); }

    // clear replace combobox text
    replaceEditor_->setEditText("");

    // set focus to find editor
    editor().setFocus();

}

//__________________________________________________
void BaseReplaceWidget::_replaceInWindow( void )
{ emit replaceInWindow( selection( false ) ); }

//__________________________________________________
void BaseReplaceWidget::_replaceInSelection( void )
{ emit replaceInSelection( selection( false ) ); }


//__________________________________________________
void BaseReplaceWidget::_addReplacedString( const QString& text  )
{
    if( text.isEmpty() ) return;
    if( _replacedStrings().find( text ) == _replacedStrings().end() )
    {
        _replacedStrings().insert( text );
        replaceEditor_->addItem( text );
    }
}
