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

#include "MimeTypeIconProvider.h"
#include "IconEngine.h"

//__________________________________________________________________
MimeTypeIconProvider::MimeTypeIconProvider( QObject* parent ):
    QObject( parent ),
    Counter( "MimeTypeIconProvider" )
{

    // source code
    iconNames_.insert( "C", "text-x-csrc" );
    iconNames_.insert( "cxx", "text-x-c++src" );
    iconNames_.insert( "cpp", "text-x-c++src" );
    iconNames_.insert( "h", "text-x-chdr" );

    // shell scripts
    iconNames_.insert( "sh", "application-x-shellscript" );
    iconNames_.insert( "csh", "application-x-shellscript" );
    iconNames_.insert( "tcsh", "application-x-shellscript" );
    iconNames_.insert( "bash", "application-x-shellscript" );
    iconNames_.insert( "zsh", "application-x-shellscript" );
    iconNames_.insert( "pl", "application-x-perl" );

    // double check
    iconNames_.insert( "la", "application-x-perl" );

    // latex
    iconNames_.insert( "tex", "text-x-tex" );
    iconNames_.insert( "sty", "text-x-tex" );
    iconNames_.insert( "bib", "text-x-bibtex" );
    iconNames_.insert( "aux", "text-plain" );
    iconNames_.insert( "eps", "image-x-eps" );
    iconNames_.insert( "ps", "application-postscript" );
    iconNames_.insert( "pdf", "application-pdf" );
    iconNames_.insert( "fig", "image-x-xfig" );

    // images
    iconNames_.insert( "png", "image-x-generic" );
    iconNames_.insert( "ico", "image-x-generic" );
    iconNames_.insert( "jpg", "image-x-generic" );
    iconNames_.insert( "jpeg", "image-x-generic" );
    iconNames_.insert( "gif", "image-x-generic" );

    iconNames_.insert( "svg", "image-svg+xml" );
    iconNames_.insert( "svgz", "image-svg+xml-compressed" );

    // videos
    iconNames_.insert( "avi", "video-x-generic" );
    iconNames_.insert( "mp4", "video-x-generic" );
    iconNames_.insert( "mkv", "video-x-generic" );

    // music
    iconNames_.insert( "mp3", "audio-x-generic" );
    iconNames_.insert( "flac", "audio-x-generic" );
    iconNames_.insert( "wav", "audio-x-generic" );
    iconNames_.insert( "ogg", "audio-x-generic" );

    // text formats
    iconNames_.insert( "txt", "text-plain" );
    iconNames_.insert( "qrc", "text-plain" );
    iconNames_.insert( "log", "text-plain" );
    iconNames_.insert( "xml", "text-xml" );
    iconNames_.insert( "html", "text-html" );
    iconNames_.insert( "css", "text-css" );
    iconNames_.insert( "php", "application-x-php" );

    // cmake
    iconNames_.insert( "cmake", "text-x-cmake" );

    // archives
    iconNames_.insert( "gz", "application-x-archive" );
    iconNames_.insert( "xz", "application-x-archive" );
    iconNames_.insert( "bz2", "application-x-archive" );
    iconNames_.insert( "tgz", "application-x-archive" );
    iconNames_.insert( "zip", "application-x-archive" );

    // misc
    iconNames_.insert( "root", "application-x-object" );
    iconNames_.insert( "o", "application-x-object" );
    iconNames_.insert( "obj", "application-x-object" );
    iconNames_.insert( "so", "application-x-object" );
    iconNames_.insert( "a", "application-x-archive" );
    iconNames_.insert( "exe", "application-x-ms-dos-executable" );

}

//__________________________________________________________________
const QIcon& MimeTypeIconProvider::icon( const QString& extension )
{

    {
        // find match in cache
        IconCache::const_iterator iter( icons_.find( extension ) );
        if( iter != icons_.end() ) return iter.value();
    }

    {
        // find match in icon names
        IconNameCache::const_iterator iter( iconNames_.find( extension ) );
        if( iter == iconNames_.end() ) return empty_;

        // lookup matching icon using engine
        QIcon icon( IconEngine::get( iter.value() ) );
        return icons_.insert( iter.key(), icon ).value();
    }

}
