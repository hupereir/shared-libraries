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

#include "AnimatedLineEditor.h"
#include "BaseIcons.h"
#include "BrowsedLineEditor.h"
#include "BrowsedLineEditor.moc"
#include "File.h"
#include "FileDialog.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "Util.h"
#include "Debug.h"

#include <QHBoxLayout>
#include <QToolButton>

//____________________________________________________________
BrowsedLineEditor::BrowsedLineEditor( QWidget *parent ):
    QWidget( parent ),
    Counter( "BrowsedLineEditor" ),
    acceptMode_( QFileDialog::AcceptOpen ),
    fileMode_( QFileDialog::ExistingFile )
{

    Debug::Throw( "BrowsedLineEditor::BrowsedLineEditor.\n" );

    // insert horizontal layout
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    setLayout( layout );

    // create line editor
    lineEditor_ = new Editor( this );
    layout->addWidget( lineEditor_, 1 );

    // create push_button
    QToolButton *button = new QToolButton( this );
    button->setAutoRaise( true );

    button->setIcon( IconEngine::get( ICONS::OPEN ) );
    button->setToolTip( tr( "Browse file system" ) );
    layout->addWidget( button, 0 );

    // connect push_button
    connect( button, SIGNAL( clicked() ), SLOT( _browse() ) );
}

//____________________________________________________________
void BrowsedLineEditor::setFile( const QString& file )
{ editor().setText( file ); }

//____________________________________________________________
void BrowsedLineEditor::_browse( void )
{

    Debug::Throw( "BrowsedLineEditor::_browse.\n" );
    FileDialog dialog( this );
    dialog.setAcceptMode( acceptMode_ );
    dialog.setFileMode( fileMode_ );
    if( !editor().text().isNull() ) dialog.selectFile( editor().text() );

    QString file( dialog.getFile() );
    if( !file.isNull() ) setFile( file );
    return;
}
