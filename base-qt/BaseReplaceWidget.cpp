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

#include <QLineEdit>
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
    const auto editor = qobject_cast<CustomComboBox*>( &this->editor() );
    if( editor )
    { disconnect( editor->lineEdit(), &QLineEdit::textChanged, this, &BaseReplaceWidget::_findNoIncrement ); }

    replaceEditor_->setEditable( true );
    replaceEditor_->setAutoCompletion( true, Qt::CaseSensitive );
    replaceEditor_->setNavigationEnabled( false );

    connect( replaceEditor_->lineEdit(), &QLineEdit::returnPressed, this, &BaseReplaceWidget::_replace );
    connect( replaceEditor_->lineEdit(), &QLineEdit::returnPressed, this, &BaseReplaceWidget::_updateFindComboBox );
    connect( replaceEditor_->lineEdit(), &QLineEdit::returnPressed, this, &BaseReplaceWidget::_updateReplaceComboBox );
    connect( replaceEditor_->lineEdit(), &QLineEdit::textChanged, this, &BaseReplaceWidget::replaceTextChanged );

    setTabOrder( &this->editor(), replaceEditor_ );

    // replace buttons
    QPushButton* button = new QPushButton( tr( "Replace" ), this );
    connect( button, &QAbstractButton::clicked, this, &BaseReplaceWidget::_replace );
    connect( button, &QAbstractButton::clicked, this, &BaseReplaceWidget::_updateFindComboBox );
    connect( button, &QAbstractButton::clicked, this, &BaseReplaceWidget::_updateReplaceComboBox );
    _addDisabledButton( button );
    _editorLayout().addWidget( button, 1, 2, 1, 1 );
    button->setAutoDefault( false );

    setTabOrder( &_findPreviousButton(), button );
    QPushButton* previous = button;

    // replace all button
    button = new QPushButton( tr( "Replace in" ), this );
    _addDisabledButton( button );
    _editorLayout().addWidget( button, 1, 3, 1, 1 );
    button->setAutoDefault( false );
    button->setMenu( replaceAllMenu_ = new QMenu );
    setTabOrder( previous, button );

    // replace in selection action
    QAction* action;
    replaceAllMenu_->addAction( action = new QAction( tr( "Selection" ), this ) );
    connect( action, &QAction::triggered, this, &BaseReplaceWidget::_replaceInSelection );
    connect( action, &QAction::triggered, this, &BaseReplaceWidget::_updateFindComboBox );
    connect( action, &QAction::triggered, this, &BaseReplaceWidget::_updateReplaceComboBox );
    replaceInSelectionAction_ = action;

    // replace in window action
    replaceAllMenu_->addAction( action = new QAction( tr( "Window" ), this ) );
    connect( action, &QAction::triggered, this, &BaseReplaceWidget::_replaceInWindow );
    connect( action, &QAction::triggered, this, &BaseReplaceWidget::_updateFindComboBox );
    connect( action, &QAction::triggered, this, &BaseReplaceWidget::_updateReplaceComboBox );

    connect( replaceAllMenu_, &QMenu::aboutToShow, this, &BaseReplaceWidget::menuAboutToShow );

}

//_____________________________________________________
TextSelection BaseReplaceWidget::selection( bool noIncrement ) const
{

    TextSelection out( BaseFindWidget::selection( noIncrement ) );
    out.setReplaceText( replaceEditor_->currentText() );
    return out;

}

//__________________________________________________
void BaseReplaceWidget::synchronize()
{
    Debug::Throw( "BaseReplaceWidget::synchronize.\n" );

    // base class method
    BaseFindWidget::synchronize();

    // replace editor
    replaceEditor_->clear();
    for( const auto& string:_replacedStrings() )
    { replaceEditor_->addItem( string ); }

    // clear replace combobox text
    replaceEditor_->setEditText("");

    // set focus to find editor
    editor().setFocus();

}

//__________________________________________________
void BaseReplaceWidget::_replaceInWindow()
{ emit replaceInWindow( selection( false ) ); }

//__________________________________________________
void BaseReplaceWidget::_replaceInSelection()
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
