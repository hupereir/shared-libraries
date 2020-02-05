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
    Counter( QStringLiteral("MimeTypeIconProvider") ),
    iconNames_(
    {
        // source code
        { "C", "text-x-csrc" },
        { "cc", "text-x-c++src" },
        { "cxx", "text-x-c++src" },
        { "cpp", "text-x-c++src" },
        { "h", "text-x-chdr" },

        // shell scripts
        { "sh", "application-x-shellscript" },
        { "csh", "application-x-shellscript" },
        { "tcsh", "application-x-shellscript" },
        { "bash", "application-x-shellscript" },
        { "zsh", "application-x-shellscript" },
        { "pl", "application-x-perl" },

        // double check
        { "la", "application-x-perl" },

        // latex
        { "tex", "text-x-tex" },
        { "sty", "text-x-tex" },
        { "bib", "text-x-bibtex" },
        { "aux", "text-plain" },
        { "eps", "image-x-eps" },
        { "ps", "application-postscript" },
        { "pdf", "application-pdf" },
        { "fig", "image-x-xfig" },

        // images
        { "png", "image-x-generic" },
        { "ico", "image-x-generic" },
        { "jpg", "image-x-generic" },
        { "jpeg", "image-x-generic" },
        { "gif", "image-x-generic" },

        { "svg", "image-svg+xml" },
        { "svgz", "image-svg+xml-compressed" },

        // videos
        { "avi", "video-x-generic" },
        { "mp4", "video-x-generic" },
        { "mkv", "video-x-generic" },

        // music
        { "mp3", "audio-x-generic" },
        { "flac", "audio-x-generic" },
        { "wav", "audio-x-generic" },
        { "ogg", "audio-x-generic" },
        { "cue", "application-x-cue" },

        // text formats
        { "txt", "text-plain" },
        { "qrc", "text-plain" },
        { "log", "text-plain" },
        { "xml", "text-xml" },
        { "html", "text-html" },
        { "css", "text-css" },
        { "php", "application-x-php" },

        // cmake
        { "cmake", "text-x-cmake" },

        // archives
        { "gz", "application-x-gzip" },
        { "xz", "application-zip" },
        { "bz2", "application-x-bzip" },
        { "tgz", "application-zip" },
        { "zip", "application-zip" },
        { "cbz", "application-zip" },
        { "rar", "application-x-rar" },
        { "cbr", "application-x-rar" },

        // misc
        { "a", "application-x-archive" },
        { "apk", "android-package-archive" },
        { "desktop", "application-x-desktop" },
        { "exe", "application-x-ms-dos-executable" },
        { "json", "application-json" },
        { "o", "application-x-object" },
        { "obj", "application-x-object" },
        { "root", "application-x-root" },
        { "rpm", "application-x-rpm" },
        { "srpm", "application-x-source-rpm" },
        { "so", "application-x-object" },
        { "spec", "text-x-rpm-spec" }
    } )
{}

//__________________________________________________________________
const QIcon& MimeTypeIconProvider::icon( const QString& extension )
{

    {
        // find match in cache
        auto&& iter( icons_.find( extension ) );
        if( iter != icons_.end() ) return iter.value();
    }

    {
        // find match in icon names
        auto&& iter( iconNames_.find( extension ) );
        if( iter == iconNames_.end() ) return empty_;

        // lookup matching icon using engine
        auto icon( IconEngine::get( iter.value() ) );
        return icons_.insert( iter.key(), icon ).value();
    }

}
