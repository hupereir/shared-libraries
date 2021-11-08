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

#include "Debug.h"
#include "File.h"
#include "FileDialog.h"
#include "ImageFileDialog.h"
#include "Pixmap.h"
#include "QtUtil.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QFileInfo>
#include <QUrl>
#include <QImage>
#include <QLayout>
#include <QLabel>
#include <QMimeData>
#include <QPushButton>
#include <QSplitter>

//______________________________________________________________________
ImageFileDialog::ImageFileDialog( QWidget* parent ):
    QFileDialog( parent )
{
    Debug::Throw( QStringLiteral("ImageFileDialog::ImageFileDialog.\n") );

    // no size grip, ever
    setSizeGripEnabled( false );

    // working directory
    if( !FileDialog::_workingDirectory().isEmpty() && QFileInfo( FileDialog::_workingDirectory() ).isDir() )
    { setDirectory( QDir( FileDialog::_workingDirectory() ) ); }

    Debug::Throw() << "ImageFileDialog::ImageFileDialog - working directory: " << FileDialog::_workingDirectory() << Qt::endl;
    connect( this, &QFileDialog::directoryEntered, this, &ImageFileDialog::saveWorkingDirectory );

    // add image display
    auto splitter = findChild<QSplitter*>( QStringLiteral("splitter") );
    if( splitter )
    {
        auto main( new QWidget );
        splitter->addWidget( main );

        auto vLayout = new QVBoxLayout;
        main->setLayout( vLayout );
        vLayout->setSpacing(5);
        QtUtil::setMargin(vLayout, 0);
        vLayout->addWidget( preview_ = new Label( main ), 1 );
        preview_->setAlignment( Qt::AlignCenter );
        preview_->setFrameStyle( QFrame::StyledPanel|QFrame::Sunken );

        auto hLayout = new QHBoxLayout;
        hLayout->setSpacing(5);
        QtUtil::setMargin(hLayout, 0);
        vLayout->addLayout( hLayout );

        hLayout->addWidget( automaticPreview_ = new QCheckBox( tr( "Automatic preview" ), main ) );
        automaticPreview_->setChecked( true );

        auto button = new QPushButton( tr( "Preview" ), main );
        hLayout->addWidget( button );
        connect( button, &QAbstractButton::clicked, this, &ImageFileDialog::_preview );

    } else Debug::Throw() << "ImageFileDialog::ImageFileDialog - unable to find splitter." << Qt::endl;

    connect( this, QOverload<const QString&>::of( &ImageFileDialog::currentChanged ), this, &ImageFileDialog::_currentChanged );

}

//______________________________________________________________________
ImageFileDialog::Label::Label( QWidget* parent ):
    QLabel( parent ),
    Counter( QStringLiteral("ImageFileDialog::Label") )
{ setAcceptDrops( true ); }

//______________________________________________________________________
void ImageFileDialog::Label::dragEnterEvent( QDragEnterEvent *event )
{
    Debug::Throw( QStringLiteral("ImageFileDialog::Label::dragEnterEvent.\n") );
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

//______________________________________________________________________
void ImageFileDialog::Label::dropEvent( QDropEvent *event )
{

    Debug::Throw( QStringLiteral("ImageFileDialog::Label::dropEvent.\n") );

    // check if event is valid
    if( !event->mimeData()->hasUrls() ) return;

    // loop over event URLs
    for( const auto& url:event->mimeData()->urls() )
    {
        QFileInfo fileInfo( url.toLocalFile() );
        if( fileInfo.exists() )
        {
            auto dialog( static_cast<ImageFileDialog*>( window() ) );
            if( fileInfo.isDir() ) dialog->setDirectory( fileInfo.filePath() );
            else {

                dialog->setDirectory( fileInfo.path() );
                dialog->selectFile( File( fileInfo.fileName() ) );
                if( dialog->automaticPreview_ && dialog->automaticPreview_->isChecked() )
                { dialog->_currentChanged( fileInfo.filePath() ); }

            }
            event->acceptProposedAction();
            return;
        }
    }

}

//______________________________________________________________________
void ImageFileDialog::saveWorkingDirectory( const QString &directory )
{
    Debug::Throw() << "ImageFileDialog::saveWorkingDirectory - directory: " << directory << Qt::endl;
    FileDialog::setWorkingDirectory( File( QFileInfo( directory ).absolutePath() ) );
}

//______________________________________________________________________
void ImageFileDialog::_currentChanged( const QString& value )
{
    Debug::Throw( QStringLiteral("ImageFileDialog::_currentChanged.\n") );
    currentPath_ = File( value );
    if( automaticPreview_ && automaticPreview_->isChecked() ) _preview();
}

//______________________________________________________________________
void ImageFileDialog::_preview()
{

    Debug::Throw( QStringLiteral("ImageFileDialog::_preview.\n") );

    // nothing if no preview
    if( !preview_ ) return;

    // try load svg
    Pixmap pixmap;

    if( currentPath_.endsWith( ".svg" ) || currentPath_.endsWith( ".svgz" ) )
    {

        QSize size( 0.8*preview_->width(), 0.8*preview_->height() );
        pixmap = Pixmap( QIcon( currentPath_ ).pixmap( size ) );

    } else pixmap = Pixmap( currentPath_ );

    // check and scale
    if( pixmap.isNull() ) preview_->setPixmap( QPixmap() );
    else {

        if( pixmap.width() > preview_->width()*0.8 || pixmap.height() > preview_->height()*0.8 )
        { pixmap = Pixmap( pixmap.scaled( int(preview_->width()*0.8), int(preview_->height()*0.8), Qt::KeepAspectRatio, Qt::SmoothTransformation ) ); }
        preview_->setPixmap( pixmap );

    }

}
