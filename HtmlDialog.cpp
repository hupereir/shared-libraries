
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

#include "HtmlDialog.h"

#include "FileDialog.h"
#include "GridLayout.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlOptions.h"

#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QToolButton>

//___________________________________________________
HtmlDialog::HtmlDialog( QWidget* parent ):
    CustomDialog( parent, OkButton|CancelButton|Separator ),
    tabWidget_( 0 )
{

    Debug::Throw( "HtmlDialog::HtmlDialog.\n" );

    setOptionName( "HTML_DIALOG" );

    // file
    GridLayout *gridLayout = new GridLayout();
    gridLayout->setMargin(0);
    gridLayout->setSpacing(2);
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment(  0, Qt::AlignRight|Qt::AlignVCenter );

    mainLayout().addItem( gridLayout );

    gridLayout->addWidget( new QLabel( "File:", this ) );
    gridLayout->addWidget( destinationEditor_ = new BrowsedLineEditor( this ) );
    destinationEditor_->setFileMode( QFileDialog::AnyFile );

    gridLayout->addWidget( new QLabel( "Command:", this ) );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(2);
    hLayout->setMargin(0);
    gridLayout->addLayout( hLayout );

    // command
    hLayout->addWidget( commandEditor_ = new CustomComboBox( this ) );
    commandEditor_->setEditable( true );

    // browse command button associated to the CustomComboBox
    QToolButton* button = new QToolButton( this );
    button->setIcon( IconEngine::get( ICONS::OPEN ) );
    button->setAutoRaise( true );
    hLayout->addWidget( button );
    connect( button, SIGNAL( clicked() ), SLOT( _browseCommand() ) );

    // change button text
    okButton().setText( "&Export" );
    okButton().setIcon( IconEngine::get( ICONS::HTML ));

    // make sure HTML command is saved
    XmlOptions::get().keep( "HTML_COMMAND" );

    Options::List commands( XmlOptions::get().specialOptions( "HTML_COMMAND" ) );
    for( Options::List::iterator iter = commands.begin(); iter != commands.end(); ++iter )
    { addCommand( iter->raw() ); }

    connect( &okButton(), SIGNAL( clicked() ), SLOT( _saveCommands( void ) ) );

}

//__________________________________________________
void HtmlDialog::setOptionWidgets( QList<QWidget*> widgets )
{

    Debug::Throw( "HtmlDialog::setOptionWidgets\n" );
    if( widgets.isEmpty() ) return;

    assert( !tabWidget_ );
    tabWidget_ = new QTabWidget( this );
    mainLayout().addWidget( tabWidget_ );

    foreach( QWidget* widget, widgets )
    { tabWidget_->addTab( widget, widget->windowTitle() ); }

}

//__________________________________________________
void HtmlDialog::_browseCommand( void )
{

    Debug::Throw( "HtmlDialog::_browseCommand.\n" );

    // open FileDialog
    QString file( FileDialog( this ).getFile() );
    if( !file.isNull() )
    {
        commandEditor_->setEditText( file );
        commandEditor_->addItem( file );
    }

    return;

}

//__________________________________________________
void HtmlDialog::_saveCommands( void )
{
    Debug::Throw( "HtmlDialog::_saveCommands.\n" );
    XmlOptions::get().add( "HTML_COMMAND", Option( command(), Option::Recordable|Option::Current ) );
}
