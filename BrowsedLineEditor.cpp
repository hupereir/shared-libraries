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

/*!
\file BrowsedLineEditor.cpp
\brief combined QLineEdit + associated browse button
line edit is also customized to have faster popup menu
result of the file dialog associated to the button is stored into the
LineEditor object
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QHBoxLayout>
#include <QToolButton>

#include "AnimatedLineEditor.h"
#include "BaseIcons.h"
#include "BrowsedLineEditor.h"
#include "File.h"
#include "FileDialog.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "Util.h"
#include "Debug.h"

using namespace std;

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
    button->setToolTip( "Browse file system" );
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
    dialog.setAcceptMode( _acceptMode() );
    dialog.setFileMode( _fileMode() );
    if( !editor().text().isNull() ) dialog.selectFile( editor().text() );

    QString file( dialog.getFile() );
    if( !file.isNull() ) setFile( file );
    return;
}
