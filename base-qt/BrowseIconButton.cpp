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

#include "LineEditor.h"
#include "CustomPixmap.h"
#include "File.h"
#include "IconSize.h"
#include "BrowseIconButton.h"
#include "ImageFileDialog.h"
#include "InformationDialog.h"
#include "XmlOptions.h"
#include "QtUtil.h"

#include <QMimeData>
#include <QUrl>

//_____________________________________________
BrowseIconButton::BrowseIconButton( QWidget* parent, const QString& file):
    CustomToolButton( parent )
{

    setIconSize( IconSize( IconSize::Huge ) );
    setAutoRaise( false );
    setFile( file, false );
    connect( this, SIGNAL(clicked()), SLOT(_browse()) );
    setAcceptDrops( true );

}


//_____________________________________________
bool BrowseIconButton::setFile( const QString& file, bool check )
{

    Debug::Throw() << "BrowseIconButton::setFile - " << file << endl;

    // do nothing for empty file
    if( file.isEmpty() ) return false;

    // load pixmap
    CustomPixmap pixmap( file );

    // update file if pixmap is valid or current file is undefined
    if( !pixmap.isNull() || file_.isEmpty() )
    { file_ = file; }

    // update pixmap if valid
    if( !pixmap.isNull() )
    {

        // resize pixmap
        if( pixmap.size() != IconSize( IconSize::Huge ) )
            pixmap = pixmap.scaled( IconSize( IconSize::Huge ), Qt::KeepAspectRatio, Qt::SmoothTransformation );

        setIcon( pixmap );
        return true;
    }

    // popup dialog if invalid
    if( check )
    { InformationDialog( this, QString( tr( "Invalid icon file %1" ) ).arg( file ) ).exec(); }

    // if file, set pixmap to empty
    if( noIconPixmap_.isNull() ) {
        noIconPixmap_ = CustomPixmap( IconSize( IconSize::Huge ), CustomPixmap::Transparent );
        setIcon( noIconPixmap_ );
    }

    return false;

}

//_____________________________________________
void BrowseIconButton::_browse( void )
{
    Debug::Throw( "BrowseIconButton::_Browse.\n" );

    ImageFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::AnyFile );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setWindowTitle( tr( "Open" ) );
    QtUtil::centerOnParent( &dialog );

    if( !file_.isEmpty() )
    {

        // warning: inneficient
        File path( File( file_ ).path() );
        if( path.exists() && path.isDirectory() ) dialog.setDirectory( path );
        dialog.selectFile( file_ );

    }

    dialog.show();
    if( dialog.exec() == QDialog::Rejected ) return;

    // retrieve selected files
    QStringList files( dialog.selectedFiles() );

    // check file size
    if( files.size() > 1 )
    {
        InformationDialog( this, tr( "Too many files selected." ) ).exec();
        return;
    }

    if( files.size() < 1 )
    {
        InformationDialog( this, tr( "No file selected." ) ).exec();
        return;
    }

    setFile( files.front(), true );
    return;
}

//______________________________________________________________________
void BrowseIconButton::dragEnterEvent( QDragEnterEvent *event )
{
    Debug::Throw( "BrowseIconButton::dragEnterEvent.\n" );
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

//______________________________________________________________________
void BrowseIconButton::dropEvent( QDropEvent *event )
{

    Debug::Throw( "BrowseIconButton::dropEvent.\n" );

    // check if event is valid
    if( !event->mimeData()->hasUrls() ) return;

    // loop over event URLs
    for( const auto& url:event->mimeData()->urls() )
    {
        QFileInfo fileInfo( url.toLocalFile() );
        if( fileInfo.exists() && setFile( fileInfo.filePath(), true ) ) event->acceptProposedAction();
    }

    return;

}
