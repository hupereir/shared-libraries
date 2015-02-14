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

#include "BaseIconNames.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "LineEditor.h"
#include "Debug.h"
#include "BaseFindWidget.h"

#include <QFrame>
#include <QPushButton>

//________________________________________________________________________
QOrderedSet<QString>& BaseFindWidget::_searchedStrings( void )
{
    static QOrderedSet<QString> strings;
    return strings;
}

//________________________________________________________________________
BaseFindWidget::BaseFindWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "BaseFindWidget" )
{
    Debug::Throw( "BaseFindWidget::BaseFindWidget.\n" );

    // create vbox layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin( 0 );
    layout()->setSpacing( 5 );

    // edition
    layout()->addItem( editorLayout_ = new QGridLayout() );
    editorLayout_->setMargin( 0 );
    editorLayout_->setSpacing( 5 );

    // add editor
    QLabel *label = new QLabel( tr( "Text to find:" ), this );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    editorLayout_->addWidget( label, 0, 0, 1, 1 );
    editorLayout_->addWidget( editor_ = new CustomComboBox( this ), 0, 1, 1, 1 );
    label->setBuddy( editor_ );
    editor_->setEditable( true );
    editor_->setAutoCompletion( true, Qt::CaseSensitive );

    connect( editor_->lineEdit(), SIGNAL(returnPressed()), SLOT(_find()) );
    connect( editor_->lineEdit(), SIGNAL(returnPressed()), SLOT(_updateFindComboBox()) );
    connect( editor_->lineEdit(), SIGNAL(textChanged(QString)), SLOT(_updateButtons(QString)) );
    connect( editor_->lineEdit(), SIGNAL(textChanged(QString)), SLOT(_findNoIncrement()) );

    // locations
    GridLayout* gridLayout( new GridLayout() );
    gridLayout->setSpacing( 5 );
    gridLayout->setMargin( 0 );
    gridLayout->setMaxCount( 2 );

    layout()->addItem( gridLayout );

    // insert checkboxes
    gridLayout->addWidget( backwardCheckbox_ = new QCheckBox( tr( "Search backward" ), this ) );
    gridLayout->addWidget( caseSensitiveCheckbox_ = new QCheckBox( tr( "Case sensitive" ), this ) );
    gridLayout->addWidget( regexpCheckbox_ = new QCheckBox( tr( "Regular expresion" ), this ) );
    gridLayout->addWidget( entireWordCheckbox_ = new QCheckBox( tr( "Entire word" ), this ) );
    connect( regexpCheckbox_, SIGNAL(toggled(bool)), SLOT(_regExpChecked(bool)) );

    // tooltips
    backwardCheckbox_->setToolTip( tr( "Perform search backward" ) );
    caseSensitiveCheckbox_->setToolTip( tr( "Case sensitive search" ) );
    regexpCheckbox_->setToolTip( tr( "Search text using regular expression" ) );

    // notification label
    layout()->addWidget( label_ = new QLabel( this ) );
    label_->setMargin( 2 );

    // location layout
    layout()->addItem( locationLayout_ = new QHBoxLayout() );
    locationLayout_->setMargin(0);
    locationLayout_->setSpacing(5);

    // horizontal separator
    QFrame* frame( new QFrame( this ) );
    frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    layout()->addWidget( frame );

    // buttons
    layout()->addItem( buttonLayout_ = new QHBoxLayout() );
    buttonLayout_->setMargin( 0 );
    buttonLayout_->setSpacing( 5 );
    buttonLayout_->addStretch(1);

    // insert Find button
    buttonLayout_->addWidget( findButton_ = new QPushButton( IconEngine::get( IconNames::Find ), tr( "Find" ), this ) );
    connect( findButton_, SIGNAL(clicked()), this, SLOT(_find()) );
    connect( findButton_, SIGNAL(clicked()), this, SLOT(_updateFindComboBox()) );
    addDisabledButton( findButton_ );
    static_cast<QPushButton*>(findButton_)->setAutoDefault( false );

    // insert Cancel button
    buttonLayout_->addWidget( closeButton_ = new QPushButton( IconEngine::get( IconNames::DialogClose ), tr( "Close" ), this ) );
    closeButton_->setShortcut( Qt::Key_Escape );
    static_cast<QPushButton*>(closeButton_)->setAutoDefault( false );

}

//________________________________________________________________________
TextSelection BaseFindWidget::selection( bool noIncrement ) const
{
    TextSelection out( editor_->currentText() );
    out.setFlag( TextSelection::Backward, backwardCheckbox_->isChecked() );
    out.setFlag( TextSelection::CaseSensitive, caseSensitiveCheckbox_->isChecked() );
    out.setFlag( TextSelection::EntireWord, entireWordCheckbox_->isChecked() );
    out.setFlag( TextSelection::RegExp, regexpCheckbox_->isChecked() );
    out.setFlag( TextSelection::NoIncrement, noIncrement );
    return out;
}

//________________________________________________________________________
void BaseFindWidget::setText( const QString& text )
{
    _addSearchedString( text );
    editor_->setEditText( text );
    editor_->lineEdit()->selectAll();
}

//________________________________________________________________________
void BaseFindWidget::enableEntireWord( bool value )
{
    if( !value ) entireWordCheckbox_->setChecked( false );
    entireWordCheckbox_->setEnabled( value );
}

//________________________________________________________________________
void BaseFindWidget::enableRegExp( bool value )
{
    if( !value ) regexpCheckbox_->setChecked( false );
    regexpCheckbox_->setEnabled( value );
}

//________________________________________________________________________
void BaseFindWidget::addDisabledButton( QAbstractButton* button )
{
    Debug::Throw( "BaseFindWidget::addDisabledButton.\n" );
    buttons_ << button;
    _updateButtons();
}

//________________________________________________________________________
void BaseFindWidget::matchFound( void )
{ label_->setText( "" ); }

//________________________________________________________________________
void BaseFindWidget::noMatchFound( void )
{ if( !editor_->currentText().isEmpty() ) label_->setText( tr( "Not found" ) ); }

//________________________________________________________________________
void BaseFindWidget::_addSearchedString( const QString& text  )
{
    Debug::Throw( "BaseFindWidget::_addSearchedString.\n" );
    if( text.isEmpty() ) return;
    if( _searchedStrings().find( text ) == _searchedStrings().end() )
    {
        _searchedStrings().insert( text );
        editor_->addItem( text );
    }
}

//________________________________________________________________________
void BaseFindWidget::synchronize( void )
{
    Debug::Throw( "BaseFindWidget::synchronize.\n" );
    editor_->clear();

    foreach( const auto& string, _searchedStrings() )
    { editor_->addItem( string ); }
}

//________________________________________________________________________
void BaseFindWidget::_regExpChecked( bool value )
{
    Debug::Throw( "BaseFindWidget::_regExpChecked.\n" );
    caseSensitiveCheckbox_->setChecked( value );
    if( value )
    {
        entireWordCheckbox_->setChecked( false );
        entireWordCheckbox_->setEnabled( false );
    } else { entireWordCheckbox_->setEnabled( true ); }

}

//________________________________________________________________________
void BaseFindWidget::_updateButtons( const QString& text )
{
    Debug::Throw( "BaseFindWidget::_updateButtons.\n" );

    const bool enabled( !( text.isNull() || text.isEmpty() ) );
    foreach( const auto& button, buttons_ )
    { button->setEnabled( enabled ); }

}
