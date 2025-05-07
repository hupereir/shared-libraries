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

#include "BrowsedLineEditor.h"
#include "BaseIconNames.h"
#include "Debug.h"
#include "File.h"
#include "FileDialog.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "LineEditorButton.h"
#include "QtUtil.h"
#include "Util.h"

#include <QLayout>
#include <QToolButton>

#include <QStandardPaths>

//____________________________________________________________
BrowsedLineEditor::BrowsedLineEditor( QWidget *parent ):
    LineEditor( parent )
{

    Debug::Throw( QStringLiteral("BrowsedLineEditor::BrowsedLineEditor.\n") );

    // create button
    auto button = new LineEditorButton;
    button->setIcon( IconEngine::get( IconNames::Open ) );
    button->setToolTip( tr( "Browse file system" ) );
    addRightWidget(button);

    // connect button
    connect( button, &QAbstractButton::clicked, this, &BrowsedLineEditor::_browse );
}

//_____________________________________________________________
void BrowsedLineEditor::setTargetApplication( const File &target )
{
    targetApplication_ = target;
    if( target.isEmpty() ) findTargetButton_.reset();
    else if( !findTargetButton_ )
    {
        // create button
        auto button = new LineEditorButton;
        button->setIcon( IconEngine::get( IconNames::Reload ) );
        button->setText( tr( "Refresh" ) );
        addRightWidget( button );
        connect( button, &QAbstractButton::clicked, this, &BrowsedLineEditor::_findTargetApplication );

        // assign
        findTargetButton_.reset( button );
    }

}

//____________________________________________________________
void BrowsedLineEditor::setFile( const QString& file )
{ setText( file ); }

//____________________________________________________________
void BrowsedLineEditor::_browse()
{

    Debug::Throw( QStringLiteral("BrowsedLineEditor::_browse.\n") );

    if( useNativeFileDialog_ )
    {

        Debug::Throw( 0, QStringLiteral("BrowsedLineEditor::_browse - using FileDialog.\n") );
        FileDialog dialog( this );
        dialog.setAcceptMode( acceptMode_ );
        dialog.setFileMode( fileMode_ );
        if( !text().isNull() ) dialog.selectFile( File( text() ) );
        QString file( dialog.getFile() );
        if( !file.isNull() ) setFile( file );

    } else {

        Debug::Throw( 0, QStringLiteral("BrowsedLineEditor::_browse - using QFileDialog.\n") );
        QFileDialog dialog(this);
        dialog.setAcceptMode( acceptMode_ );
        dialog.setFileMode( fileMode_ );
        if( !text().isNull() ) dialog.selectFile( File( text() ) );
        if( !dialog.exec() ) return;

        auto filenames( dialog.selectedFiles() );
        if( filenames.size() == 1 ) setFile( filenames.front() );
    }
}

//_____________________________________________________________
void BrowsedLineEditor::_findTargetApplication()
{
    // check if current text is valid
    File current( text() );
    if( current.exists() ) return;

    setText( QStandardPaths::findExecutable( targetApplication_ ) );
}
