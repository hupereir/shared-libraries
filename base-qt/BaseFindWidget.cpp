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
#include "BaseIconNames.h"
#include "Color.h"
#include "Debug.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "LineEditor.h"

#include <QFrame>
#include <QPushButton>
#include <QToolButton>

//________________________________________________________________________
QOrderedSet<QString>& BaseFindWidget::_searchedStrings()
{
    static QOrderedSet<QString> strings;
    return strings;
}

//________________________________________________________________________
BaseFindWidget::BaseFindWidget( QWidget* parent, bool compact ):
    AbstractFindWidget( parent ),
    Counter( "BaseFindWidget" )
{
    Debug::Throw( "BaseFindWidget::BaseFindWidget.\n" );

    // update palette
    _updateNotFoundPalette();

    // create vbox layout
    auto vLayout = new QVBoxLayout;
    setLayout( vLayout );
    vLayout->setMargin( compact ? 2:0 );
    vLayout->setSpacing( 5 );

    // edition layout
    vLayout->addItem( editorLayout_ = new QGridLayout );
    editorLayout_->setMargin( 0 );
    editorLayout_->setSpacing( 5 );

    // label and find editor
    auto label = new QLabel( tr( "Text to find:" ), this );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    editorLayout_->addWidget( label, 0, 0, 1, 1 );
    editorLayout_->addWidget( editor_ = new CustomComboBox( this ), 0, 1, 1, 1 );
    label->setBuddy( editor_ );
    editor_->setEditable( true );
    editor_->setAutoCompletion( true, Qt::CaseSensitive );
    editor_->setNavigationEnabled( false );

    connect( static_cast<LineEditor*>(editor_->lineEdit()), &LineEditor::cleared, this, &BaseFindWidget::matchFound );
    connect( editor_->lineEdit(), &QLineEdit::returnPressed, this, &BaseFindWidget::_find );
    connect( editor_->lineEdit(), &QLineEdit::returnPressed, this, &BaseFindWidget::_updateFindComboBox );
    connect( editor_->lineEdit(), &QLineEdit::textChanged, this, &BaseFindWidget::_updateButtons );
    connect( editor_->lineEdit(), &QLineEdit::textChanged, this, &BaseFindWidget::_findNoIncrement );

    // Find next button
    findNextButton_ = new QPushButton( IconEngine::get( IconNames::FindNext ), tr( "Next" ), this );
    editorLayout_->addWidget( findNextButton_, 0, 2, 1, 1 );
    connect( findNextButton_, &QAbstractButton::clicked, this, &BaseFindWidget::_findNext );
    connect( findNextButton_, &QAbstractButton::clicked, this, &BaseFindWidget::_updateFindComboBox );
    _addDisabledButton( findNextButton_ );
    static_cast<QPushButton*>(findNextButton_)->setAutoDefault( false );

    // Find previous button
    findPreviousButton_ = new QPushButton( IconEngine::get( IconNames::FindPrevious ), tr( "Previous" ), this );
    editorLayout_->addWidget( findPreviousButton_, 0, 3, 1, 1 );
    connect( findPreviousButton_, &QAbstractButton::clicked, this, &BaseFindWidget::_findPrevious );
    connect( findPreviousButton_, &QAbstractButton::clicked, this, &BaseFindWidget::_updateFindComboBox );
    _addDisabledButton( findPreviousButton_ );
    static_cast<QPushButton*>(findPreviousButton_)->setAutoDefault( false );

    // options
    editorLayout_->addWidget( label = new QLabel( tr( "Options:" ), this ), 3, 0, 1, 1 );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );

    QHBoxLayout* hLayout( new QHBoxLayout );
    hLayout->setSpacing( 5 );
    hLayout->setMargin( 0 );
    editorLayout_->addLayout( hLayout, 3, 1, 1, 3 );

    hLayout->addWidget( caseSensitiveCheckbox_ = new QCheckBox( tr( "C&ase sensitive" ), this ) );
    hLayout->addWidget( regexpCheckbox_ = new QCheckBox( tr( "Regular expresion" ), this ) );
    hLayout->addWidget( entireWordCheckbox_ = new QCheckBox( tr( "Entire word" ), this ) );
    hLayout->addStretch(1);

    connect( regexpCheckbox_, &QAbstractButton::toggled, this, &BaseFindWidget::_regExpChecked );

    // tooltips
    // backwardCheckbox_->setToolTip( tr( "Perform search backward" ) );
    caseSensitiveCheckbox_->setToolTip( tr( "Case sensitive search" ) );
    regexpCheckbox_->setToolTip( tr( "Search text using regular expression" ) );

    if( !compact )
    {
        // stretch
        vLayout->addStretch(1);

        // horizontal separator
        auto frame = new QFrame( this );
        frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
        vLayout->addWidget( frame );
    }

    // close button
    if( compact )
    {

        closeButton_ = new QToolButton( this );
        static_cast<QToolButton*>(closeButton_)->setAutoRaise( true );
        closeButton_->setIcon( IconEngine::get( IconNames::DialogClose ) );
        editorLayout_->addWidget( closeButton_, 0, 4, 1, 1 );
        connect( closeButton_, &QAbstractButton::clicked,this,  &QWidget::hide );

    } else {

        auto hLayout = new QHBoxLayout;
        hLayout->setMargin( 0 );
        hLayout->setSpacing( 5 );
        vLayout->addItem( hLayout );

        hLayout->addStretch(1);
        hLayout->addWidget( closeButton_ = new QPushButton( IconEngine::get( IconNames::DialogClose ), tr( "Close" ), this ) );
        closeButton_->setShortcut( Qt::Key_Escape );
        static_cast<QPushButton*>(closeButton_)->setAutoDefault( false );

    }
}

//________________________________________________________________________
TextSelection BaseFindWidget::selection( bool noIncrement ) const
{
    TextSelection out( editor_->currentText() );
    out.setFlag( TextSelection::Backward, findBackward_ );
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
    entireWordCheckbox_->setVisible( value );
}

//________________________________________________________________________
void BaseFindWidget::enableRegExp( bool value )
{
    if( !value ) regexpCheckbox_->setChecked( false );
    regexpCheckbox_->setEnabled( value );
}

//________________________________________________________________________
void BaseFindWidget::matchFound()
{ editor_->setPalette( palette() ); }

//________________________________________________________________________
void BaseFindWidget::noMatchFound()
{
    if( !editor_->currentText().isEmpty() )
    { editor_->setPalette( notFoundPalette_ ); }
}

//________________________________________________________________________
void BaseFindWidget::changeEvent( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::PaletteChange:
        _updateNotFoundPalette();
        break;

        default: break;
    }

    return QWidget::changeEvent( event );
}

//________________________________________________________________________
void BaseFindWidget::_addDisabledButton( QAbstractButton* button )
{
    Debug::Throw( "BaseFindWidget::_addDisabledButton.\n" );
    buttons_.append( button );
    _updateButtons();
}

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
void BaseFindWidget::synchronize()
{
    Debug::Throw( "BaseFindWidget::synchronize.\n" );
    editor_->clear();

    for( const auto& string:_searchedStrings() )
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

    const bool enabled( !( text.isEmpty() ) );
    for( const auto& button:buttons_ )
    { button->setEnabled( enabled ); }

}

//________________________________________________________________________
void BaseFindWidget::_updateNotFoundPalette()
{
    notFoundPalette_ = palette();
    notFoundPalette_.setColor( QPalette::Base,
        Base::Color( palette().color( QPalette::Base ) ).merge(
        Qt::red, 0.95 ) );
}
