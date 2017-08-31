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
#include "BrowsedLineEditor.h"
#include "File.h"
#include "FileDialog.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "LineEditor.h"
#include "Util.h"
#include "Debug.h"

#include <QLayout>
#include <QToolButton>

#if QT_VERSION < 0x050000
#include <QProcessEnvironment>
#else
#include <QStandardPaths>
#endif

//____________________________________________________________
BrowsedLineEditor::BrowsedLineEditor( QWidget *parent ):
    QWidget( parent ),
    Counter( "BrowsedLineEditor" )
{

    Debug::Throw( "BrowsedLineEditor::BrowsedLineEditor.\n" );

    // insert horizontal layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(2);
    setLayout( layout );

    // create line editor
    lineEditor_ = new Editor( this );
    layout->addWidget( lineEditor_, 1 );

    // create button
    QToolButton *button = new QToolButton( this );
    button->setAutoRaise( true );

    button->setIcon( IconEngine::get( IconNames::Open ) );
    button->setToolTip( tr( "Browse file system" ) );
    layout->addWidget( button, 0 );

    // connect button
    connect( button, SIGNAL(clicked()), SLOT(_browse()) );
}

//_____________________________________________________________
void BrowsedLineEditor::setTargetApplication( File target )
{
    targetApplication_ = target;
    if( target.isEmpty() ) findTargetButton_.reset();
    else if( !findTargetButton_ )
    {
        auto button = new QToolButton( this );
        button->setAutoRaise( true );
        button->setIcon( IconEngine::get( IconNames::Reload ) );
        button->setText( tr( "Refresh" ) );
        layout()->addWidget( button );
        connect( button, SIGNAL(clicked()), SLOT(_findTargetApplication()) );

        findTargetButton_.reset( button );

    }

}

//____________________________________________________________
void BrowsedLineEditor::setFile( const QString& file )
{ editor().setText( file ); }

//____________________________________________________________
void BrowsedLineEditor::_browse()
{

    Debug::Throw( "BrowsedLineEditor::_browse.\n" );
    FileDialog dialog( this );
    dialog.setAcceptMode( acceptMode_ );
    dialog.setFileMode( fileMode_ );
    if( !editor().text().isNull() ) dialog.selectFile( File( editor().text() ) );

    QString file( dialog.getFile() );
    if( !file.isNull() ) setFile( file );
    return;
}

//_____________________________________________________________
void BrowsedLineEditor::_findTargetApplication()
{

    // check if current text is valid
    File current( editor().text() );
    if( current.exists() ) return;

    // path list
    #if QT_VERSION < 0x050000

    // get path list from environment
    auto environment = QProcessEnvironment::systemEnvironment();
    auto path = environment.value( "path" );
    if( path.isEmpty() ) path = environment.value( "PATH" );
    if( path.isEmpty() ) return;

    // split
    File::List pathList;
    auto list = path.split(":", QString::SkipEmptyParts);
    std::transform( list.begin(), list.end(), std::back_inserter( pathList ),
        []( const QString& item ) { return File( item ); } );

    if( pathList.empty() ) return;

    // find
    File found( targetApplication_.find( pathList ) );

    #else

    File found( QStandardPaths::findExecutable( targetApplication_ ) );

    #endif

    editor().setText( found );

}
