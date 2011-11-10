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
\file IconBrowsedButton.cpp
\brief icon browser. The icon is displayed as the button label
\author Hugo Pereira
\version $Revision$
\date $Date$
*/


#include <QUrl>

#include "LineEditor.h"
#include "CustomPixmap.h"
#include "File.h"
#include "IconSize.h"
#include "IconBrowsedButton.h"
#include "ImageFileDialog.h"
#include "InformationDialog.h"
#include "XmlOptions.h"
#include "QtUtil.h"



//_____________________________________________
const QString IconBrowsedButton::NO_ICON = "none";

//_____________________________________________
IconBrowsedButton::IconBrowsedButton( QWidget* parent, const QString& file):
    CustomToolButton( parent ),
    file_( NO_ICON )
{

    setIconSize( IconSize( IconSize::Huge ) );
    setAutoRaise( false );
    setFile( file, false );
    connect( this, SIGNAL( clicked() ), SLOT( _browse() ) );
    setAcceptDrops( true );

}


//_____________________________________________
bool IconBrowsedButton::setFile( const QString& file, const bool& check )
{

    Debug::Throw() << "IconBrowsedButton::setFile - " << file << endl;

    // load pixmap
    CustomPixmap pixmap( file );

    // update file if pixmap is valid or current file is undefined
    if( !pixmap.isNull() || file_ == NO_ICON )
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
    {
        QString buffer;
        QTextStream( &buffer ) << "invalid icon file " << file;
        InformationDialog( this, buffer ).exec();
    }

    // if file, set pixmap to empty
    if( noIconPixmap_.isNull() ) {
        noIconPixmap_ = CustomPixmap().empty( IconSize( IconSize::Huge ) );
        setIcon( noIconPixmap_ );
    }

    return false;

}

//_____________________________________________
void IconBrowsedButton::_browse( void )
{
    Debug::Throw( "IconBrowsedButton::_Browse.\n" );

    ImageFileDialog dialog( this );
    dialog.setFileMode( QFileDialog::AnyFile );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setWindowTitle( "Open" );
    QtUtil::centerOnParent( &dialog );

    if( file_ != NO_ICON ) {

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
        InformationDialog( this, "Too many files selected." ).exec();
        return;
    }

    if( files.size() < 1 )
    {
        InformationDialog( this, "No file selected." ).exec();
        return;
    }

    setFile( files.front(), true );
    return;
}

//______________________________________________________________________
void IconBrowsedButton::dragEnterEvent( QDragEnterEvent *event )
{
    Debug::Throw( "IconBrowsedButton::dragEnterEvent.\n" );
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

//______________________________________________________________________
void IconBrowsedButton::dropEvent( QDropEvent *event )
{

    Debug::Throw( "IconBrowsedButton::dropEvent.\n" );

    // check if event is valid
    if( !event->mimeData()->hasUrls() ) return;

    // loop over event URLs
    QList<QUrl> urls( event->mimeData()->urls() );
    for( QList<QUrl>::const_iterator iter = urls.begin(); iter != urls.end(); ++iter )
    {
        QFileInfo file_info( iter->toLocalFile() );
        if( file_info.exists() && setFile( file_info.filePath(), true ) ) event->acceptProposedAction();
    }

    return;

}
