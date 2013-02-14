// $Id$

/******************************************************************************
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

#include "BaseIcons.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "LineEditor.h"
#include "Debug.h"
#include "BaseFindDialog.h"

#include <QFrame>
#include <QPushButton>

//________________________________________________________________________
QOrderedSet<QString>& BaseFindDialog::_searchedStrings( void )
{
    static QOrderedSet<QString> strings;
    return strings;
}

//________________________________________________________________________
BaseFindDialog::BaseFindDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseDialog( parent, flags ),
    Counter( "BaseFindDialog" )
{
    Debug::Throw( "BaseFindDialog::BaseFindDialog.\n" );
    setOptionName( "FIND_DIALOG" );

    // set dialog title
    setWindowTitle( "Find" );

    // create vbox layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin( 10 );
    layout()->setSpacing( 5 );

    // edition
    layout()->addItem( editorLayout_ = new QGridLayout() );
    _editorLayout().setMargin( 0 );
    _editorLayout().setSpacing( 5 );

    // add editor
    QLabel *label = new QLabel( "&Text to find:", this );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    _editorLayout().addWidget( label, 0, 0, 1, 1 );
    _editorLayout().addWidget( editor_ = new CustomComboBox( this ), 0, 1, 1, 1 );
    label->setBuddy( &editor() );
    editor().setEditable( true );
    editor().setAutoCompletion( true, Qt::CaseSensitive );

    connect( editor().lineEdit(), SIGNAL(returnPressed()), SLOT( _find( void ) ) );
    connect( editor().lineEdit(), SIGNAL(returnPressed()), SLOT( _updateFindComboBox( void ) ) );
    connect( editor().lineEdit(), SIGNAL(textChanged( const QString& ) ), SLOT( _updateButtons( const QString& ) ) );
    connect( editor().lineEdit(), SIGNAL(textChanged( const QString& ) ), SLOT( _findNoIncrement( void ) ) );

    // locations
    GridLayout* gridLayout( new GridLayout() );
    gridLayout->setSpacing( 5 );
    gridLayout->setMargin( 0 );
    gridLayout->setMaxCount( 2 );

    layout()->addItem( gridLayout );

    // insert checkboxes
    gridLayout->addWidget( backwardCheckbox_ = new QCheckBox( "&Search backward", this ) );
    gridLayout->addWidget( caseSensitiveCheckbox_ = new QCheckBox( "&Case sensitive", this ) );
    gridLayout->addWidget( regexpCheckbox_ = new QCheckBox( "&Regular expresion", this ) );
    gridLayout->addWidget( entireWordCheckbox_ = new QCheckBox( "&Entire word", this ) );
    connect( regexpCheckbox_, SIGNAL( toggled( bool ) ), SLOT( _regExpChecked( bool ) ) );

    // tooltips
    backwardCheckbox_->setToolTip( "Perform search backward" );
    caseSensitiveCheckbox_->setToolTip( "Case sensitive search" );
    regexpCheckbox_->setToolTip( "Search text using regular expression" );

    // notification label
    layout()->addWidget( label_ = new QLabel( this ) );
    label_->setMargin( 2 );

    // location layout
    layout()->addItem( locationLayout_ = new QHBoxLayout() );
    _locationLayout().setMargin(0);
    _locationLayout().setSpacing(5);

    // horizontal separator
    QFrame* frame( new QFrame( this ) );
    frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    layout()->addWidget( frame );

    // buttons
    layout()->addItem( buttonLayout_ = new QHBoxLayout() );
    _buttonLayout().setMargin( 0 );
    _buttonLayout().setSpacing( 5 );
    _buttonLayout().addStretch(1);

    // insert Find button
    QPushButton *button;
    _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::FIND ), "&Find", this ) );
    connect( button, SIGNAL( clicked( void ) ), SLOT( _find( void ) ) );
    connect( button, SIGNAL( clicked( void ) ), SLOT( _updateFindComboBox( void ) ) );
    _addDisabledButton( button );
    button->setAutoDefault( false );
    findButton_ = button;

    // insert Cancel button
    _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&Close", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( close() ) );
    button->setAutoDefault( false );

    // disable buttons
    _updateButtons();

}

//________________________________________________________________________
void BaseFindDialog::synchronize( void )
{
    Debug::Throw( "BaseFindDialog::synchronize.\n" );
    editor().clear();

    for( QOrderedSet<QString>::iterator iter = _searchedStrings().begin(); iter != _searchedStrings().end(); ++iter )
    { editor().addItem( *iter ); }
}

//________________________________________________________________________
void BaseFindDialog::noMatchFound( void )
{ if( !editor().currentText().isEmpty() ) label_->setText( "Not found" ); }

//________________________________________________________________________
void BaseFindDialog::clearLabel( void )
{ label_->setText( "" ); }

//________________________________________________________________________
void BaseFindDialog::_regExpChecked( bool value )
{
    Debug::Throw( "BaseFindDialog::_regExpChecked.\n" );
    caseSensitiveCheckbox_->setChecked( value );
    if( value )
    {
        entireWordCheckbox_->setChecked( false );
        entireWordCheckbox_->setEnabled( false );
    } else { entireWordCheckbox_->setEnabled( true ); }

}

//________________________________________________________________________
void BaseFindDialog::_updateButtons( const QString& text )
{
    Debug::Throw( "BaseFindDialog::_updateButtons.\n" );

    bool enabled( !( text.isNull() || text.isEmpty() ) );

    for( QList< QAbstractButton* >::iterator iter = _disabledButtons().begin(); iter != _disabledButtons().end(); ++iter )
    { (*iter)->setEnabled( enabled ); }

    Debug::Throw( "BaseFindDialog::_updateButtons - done.\n" );
}
