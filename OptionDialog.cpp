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
* ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA   02111-1307 USA
*
*
*******************************************************************************/

#include "OptionDialog.h"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "Singleton.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLayout>

//__________________________________________________________________________
OptionDialog::OptionDialog( QWidget* parent ):
CustomDialog( parent, CloseButton )
{

    Debug::Throw( "OptionDialog::OptionDialog.\n" );
    setWindowTitle( "Runtime Options" );
    setOptionName( "OPTION_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(5);

    // tell dialog to delete when close
    setAttribute( Qt::WA_DeleteOnClose );

    // set model editable
    model_.setReadOnly( false );

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setSpacing(20);
    layout->setMargin(5);
    mainLayout().addLayout( layout );

    //! try load Question icon
    const QPixmap pixmap( IconEngine::get( ICONS::WARNING ).pixmap( iconSize() ) );
    QLabel* label = new QLabel( this );
    label->setPixmap( pixmap );
    layout->addWidget( label );

    layout->addWidget( label = new QLabel(
        "This dialog allows one to modify options manually by editting them in the list. "
        "The modifications are directly applied to the application. The reload button allows "
        "one to restore the state of the application at the time the dialog was oppened.", this ), 1 );

    label->setWordWrap( true );

    // insert list
    mainLayout().addWidget( list_ = new TreeView( this ) );
    _list().setModel( &model_ );
    _list().setRootIsDecorated( true );
    _list().setItemDelegate( new TextEditionDelegate( this ) );
    _list().setIconSize( QSize( 16, 16 ) );

    // store current option state as backup and update model
    backupOptions_ = XmlOptions::get();
    _reload();

    // connections
    connect( &model_, SIGNAL( optionModified( OptionPair ) ), SLOT( _optionModified( OptionPair ) ) );
    connect( &model_, SIGNAL( specialOptionModified( OptionPair ) ), SLOT( _specialOptionModified( OptionPair ) ) );
    connect( this, SIGNAL( configurationChanged() ), Singleton::get().application(), SIGNAL( configurationChanged() ) );

    // insert OK and Cancel button
    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( _reload() ) );
    button->setAutoDefault( false );

}

//______________________________________________________________
void OptionDialog::_reload( void )
{

    Debug::Throw( "OptionDialog::_reload.\n" );

    // retrieve all special options
    const Options::SpecialMap specialOptions( backupOptions_.specialOptions() );
    for( Options::SpecialMap::const_iterator iter = specialOptions.begin(); iter != specialOptions.end(); ++iter )
    {
        model_.add( OptionPair( iter.key(), "" ) );
        OptionModel::List options;
        foreach( const Option& option, iter.value() )
        { model_.add( OptionPair( iter.key(), option ) ); }

    }

    // retrieve normal options
    const Options::Map& options( backupOptions_.options() );
    OptionModel::List optionList;
    for( Options::Map::const_iterator iter = options.begin(); iter != options.end(); ++iter )
    { optionList << OptionPair( iter.key(), iter.value() ); }
    model_.add( optionList );

    _list().resizeColumns();
    _list().setColumnWidth( OptionModel::Current, IconSize::Small+4 );

    // check signal
    if( !( XmlOptions::get() == backupOptions_ ) )
    {
        XmlOptions::get() = backupOptions_;
        emit configurationChanged();
    }

}

//______________________________________________________________
void OptionDialog::_optionModified( OptionPair option )
{
    Debug::Throw() << "OptionDialog::_optionModified - " << option.first << " value: " << option.second.raw() << endl;
    if( XmlOptions::get().raw( option.first ) != option.second.raw() )
    {
        XmlOptions::get().setRaw( option.first, option.second.raw() );
        emit configurationChanged();
    }

    return;

}

//______________________________________________________________
void OptionDialog::_specialOptionModified( OptionPair option )
{
    Debug::Throw() << "OptionDialog::_specialOptionModified - " << option.first << endl;

    // find all matching options from model
    QModelIndex index( model_.index( OptionPair( option.first, "" ) ) );
    assert( index.isValid() );

    OptionModel::List values( model_.children( index ) );
    XmlOptions::get().clearSpecialOptions( option.first );

    if( !values.empty() )
    {

        XmlOptions::get().keep( option.first );
        foreach( const OptionPair& optionPair, values )
        {
            if( !optionPair.second.raw().isEmpty() )
            { XmlOptions::get().add( optionPair.first, optionPair.second ); }
        }

    }

}
