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
BaseReplaceWidget::BaseReplaceWidget( QWidget* parent, bool compact ):
    BaseFindWidget( parent, compact )
{
    Debug::Throw( "BaseReplaceWidget::BaseReplaceWidget.\n" );

    // replace label and editor
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

    // replace buttons
    replaceButton_ = new QPushButton( tr( "Replace" ), this );
    connect( replaceButton_, SIGNAL(clicked()), this, SLOT(_replace()) );
    connect( replaceButton_, SIGNAL(clicked()), this, SLOT(_updateFindComboBox()) );
    connect( replaceButton_, SIGNAL(clicked()), this, SLOT(_updateReplaceComboBox()) );
    _addDisabledButton( replaceButton_ );
    _editorLayout().addWidget( replaceButton_, 1, 2, 1, 1 );
    static_cast<QPushButton*>(replaceButton_)->setAutoDefault( false );

    // location label
    _editorLayout().addWidget( label = new QLabel( tr( "Replace in:" ), this ), 2, 0, 1, 1 );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );

    // location layout
    locationLayout_ = new QHBoxLayout();
    locationLayout_->setMargin(0);
    locationLayout_->setSpacing(5);
    _editorLayout().addLayout( locationLayout_, 2, 1, 1, 1 );

    // location buttons
    QPushButton *button( nullptr );

    // insert selection button
    _locationLayout().addWidget( button = new QPushButton( tr( "Selection" ), this ) );
    button->setAutoDefault( false );
    connect( button, SIGNAL(clicked()), this, SLOT(_replaceInSelection()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateFindComboBox()) );
    connect( button, SIGNAL(clicked()), this, SLOT(_updateReplaceComboBox()) );
    button->setToolTip( tr( "Replace all occurence of the search string in selected text" ) );
    _addDisabledButton( button );
    button->setAutoDefault( false );

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
