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
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
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
    iconNames_.insert( "C", "text-x-csrc.png" );
    iconNames_.insert( "cxx", "text-x-c++src.png" );
    iconNames_.insert( "cpp", "text-x-c++src.png" );
    iconNames_.insert( "h", "text-x-chdr.png" );

    // shell scripts
    iconNames_.insert( "sh", "application-x-shellscript.png" );
    iconNames_.insert( "csh", "application-x-shellscript.png" );
    iconNames_.insert( "tcsh", "application-x-shellscript.png" );
    iconNames_.insert( "bash", "application-x-shellscript.png" );
    iconNames_.insert( "zsh", "application-x-shellscript.png" );
    iconNames_.insert( "pl", "application-x-perl.png" );
    iconNames_.insert( "la", "application-x-perl.png" );

    // latex
    iconNames_.insert( "tex", "text-x-tex.png" );
    iconNames_.insert( "sty", "text-x-tex.png" );
    iconNames_.insert( "bib", "text-x-bibtex.png" );
    iconNames_.insert( "aux", "text-plain.png" );
    iconNames_.insert( "eps", "image-x-eps.png" );
    iconNames_.insert( "ps", "application-postscript.png" );
    iconNames_.insert( "pdf", "application-pdf.png" );
    iconNames_.insert( "fig", "image-x-xfig.png" );

    // images
    iconNames_.insert( "png", "image-x-generic.png" );
    iconNames_.insert( "ico", "image-x-generic.png" );
    iconNames_.insert( "jpg", "image-x-generic.png" );
    iconNames_.insert( "gif", "image-x-generic.png" );

    // videos
    iconNames_.insert( "avi", "video-x-generic.png" );
    iconNames_.insert( "mp4", "video-x-generic.png" );
    iconNames_.insert( "mkv", "video-x-generic.png" );

    // music
    iconNames_.insert( "mp3", "audio-x-generic.png" );
    iconNames_.insert( "flac", "audio-x-generic.png" );
    iconNames_.insert( "wav", "audio-x-generic.png" );
    iconNames_.insert( "ogg", "audio-x-generic.png" );

    // text formats
    iconNames_.insert( "txt", "text-plain.png" );
    iconNames_.insert( "cmake", "text-plain.png" );
    iconNames_.insert( "qrc", "text-plain.png" );
    iconNames_.insert( "log", "text-plain.png" );
    iconNames_.insert( "xml", "text-xml.png" );
    iconNames_.insert( "html", "text-html.png" );
    iconNames_.insert( "css", "text-css.png" );
    iconNames_.insert( "php", "application-x-php.png" );

    // archives
    iconNames_.insert( "gz", "application-x-archive.png" );
    iconNames_.insert( "xz", "application-x-archive.png" );
    iconNames_.insert( "bz2", "application-x-archive.png" );
    iconNames_.insert( "tgz", "application-x-archive.png" );
    iconNames_.insert( "zip", "application-x-archive.png" );
    iconNames_.insert( "a", "application-x-archive.png" );

    // misc
    iconNames_.insert( "root", "application-x-object.png" );
    iconNames_.insert( "o", "application-x-object.png" );
    iconNames_.insert( "obj", "application-x-object.png" );
    iconNames_.insert( "so", "application-x-object.png" );

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
