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

#include "BaseIcons.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "LineEditor.h"
#include "Debug.h"
#include "BaseFindDialog.h"
#include "BaseFindDialog.moc"

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
    setWindowTitle( tr( "Find" ) );

    // create vbox layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin( 10 );
    layout()->setSpacing( 5 );

    // edition
    layout()->addItem( editorLayout_ = new QGridLayout() );
    _editorLayout().setMargin( 0 );
    _editorLayout().setSpacing( 5 );

    // add editor
    QLabel *label = new QLabel( tr( "Text to find:" ), this );
    label->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
    _editorLayout().addWidget( label, 0, 0, 1, 1 );
    _editorLayout().addWidget( editor_ = new CustomComboBox( this ), 0, 1, 1, 1 );
    label->setBuddy( &editor() );
    editor().setEditable( true );
    editor().setAutoCompletion( true, Qt::CaseSensitive );

    connect( editor().lineEdit(), SIGNAL(returnPressed()), SLOT(_find()) );
    connect( editor().lineEdit(), SIGNAL(returnPressed()), SLOT(_updateFindComboBox()) );
    connect( editor().lineEdit(), SIGNAL(textChanged(QString)), SLOT(_updateButtons(QString)) );
    connect( editor().lineEdit(), SIGNAL(textChanged(QString)), SLOT(_findNoIncrement()) );

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
    _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::FIND ), tr( "Find" ), this ) );
    connect( button, SIGNAL(clicked()), SLOT(_find()) );
    connect( button, SIGNAL(clicked()), SLOT(_updateFindComboBox()) );
    _addDisabledButton( button );
    button->setAutoDefault( false );
    findButton_ = button;

    // insert Cancel button
    _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), tr( "Close" ), this ) );
    connect( button, SIGNAL(clicked()), SLOT(close()) );
    button->setShortcut( Qt::Key_Escape );
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
void BaseFindDialog::matchFound( void )
{ label_->setText( "" ); }

//________________________________________________________________________
void BaseFindDialog::noMatchFound( void )
{ if( !editor().currentText().isEmpty() ) label_->setText( tr( "Not found" ) ); }

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
