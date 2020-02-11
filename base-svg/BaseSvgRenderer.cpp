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

#include "BaseSvgRenderer.h"

#include <QDomDocument>
#include <QFile>

#if WITH_ZLIB
#include <zlib.h>
#endif

namespace Svg
{

    //________________________________________________
    BaseSvgRenderer::BaseSvgRenderer():
        QSvgRenderer(),
        Counter( QStringLiteral("Svg::BaseSvgRenderer") )
    {}

    //________________________________________________
    void BaseSvgRenderer::createStyleSheet( QPalette palette )
    {
        palette.setCurrentColorGroup( QPalette::Active );
        styleSheet_.clear();

        auto addColor = [](const QString &name, QColor color )
        { return QStringLiteral( ".ColorScheme-%1 {\n  color:%2;\n  stop-color:%2;\n}\n" ).arg( name, color.name()); };

        styleSheet_ += addColor( QStringLiteral("Text"), palette.color( QPalette::WindowText ) );
        styleSheet_ += addColor( QStringLiteral("Background"), palette.color( QPalette::Window ) );
        styleSheet_ += addColor( QStringLiteral("Highlight"), palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( QStringLiteral("ViewText"), palette.color( QPalette::Text ) );
        styleSheet_ += addColor( QStringLiteral("ViewBackground"), palette.color( QPalette::Base ) );
        styleSheet_ += addColor( QStringLiteral("ViewHover"), palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( QStringLiteral("ViewFocus"), palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( QStringLiteral("ButtonText"), palette.color( QPalette::ButtonText ) );
        styleSheet_ += addColor( QStringLiteral("ButtonBackground"), palette.color( QPalette::Button ) );
        styleSheet_ += addColor( QStringLiteral("ButtonHover"), palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( QStringLiteral("ButtonFocus"), palette.color( QPalette::Highlight ) );
    }

    //________________________________________________
    bool BaseSvgRenderer::load( const QString& filename )
    {

        // ret
        styleSheetIsUsed_ = false;

        #if WITH_ZLIB

        // open file
        QFile in( filename );
        in.open( QIODevice::ReadOnly );
        if( !( in.isOpen() && in.isReadable() ) ) return false;

        // when zlib is available we try to uncompress the file manually before passing to QSvgRenderer
        // this will allow to alter the file and in particular to change colors using current palette.
        auto content = _tryUncompress( in );
        if( content.isEmpty() )
        {

            in.seek(0);
            content = in.readAll();

        }

        // update stylesheet
        styleSheetIsUsed_ = content.contains( "current-color-scheme" );
        if( !styleSheet_.isEmpty() && styleSheetIsUsed_ )
        {
            QDomDocument document;
            document.setContent( content );

            // create new style element
            QDomElement element = document.createElement( QStringLiteral( "style" ) );
            element.setAttribute( QStringLiteral( "type" ), QStringLiteral( "text/css" ) );
            element.setAttribute( QStringLiteral( "id" ), QStringLiteral( "current-color-scheme" ) );
            element.appendChild( document.createTextNode( styleSheet_ ) );

            // find child in document
            auto&& children( document.elementsByTagName( element.tagName() ) );
            bool replaced( false );
            for( int i = 0; i < children.size() && !replaced; ++i )
            {

                auto&& child( children.at(i) );
                if( child.toElement().attribute( QStringLiteral( "id" ) ) == QStringLiteral( "current-color-scheme" ) )
                {
                    auto parent( child.parentNode() );
                    parent.replaceChild( element, child );
                    replaced = true;
                    break;
                }
            }

            if( replaced ) content = document.toByteArray();

        }

        bool loaded( QSvgRenderer::load( content ) );

        #else

        // load filename directly
        bool loaded( QSvgRenderer::load( filename ) );

        #endif

        return loaded;

    }

    //________________________________________________
    QByteArray BaseSvgRenderer::_tryUncompress( QIODevice& in ) const
    {

        QByteArray out;
        if( !( in.isOpen() && in.isReadable() ) ) return out;

        #if WITH_ZLIB

        // initialize zlib stream
        z_stream stream;
        stream.zalloc = nullptr;
        stream.zfree = nullptr;
        stream.opaque = nullptr;
        stream.avail_in = 0;
        stream.next_in = nullptr;

        if( inflateInit2(&stream, MAX_WBITS + 16) != Z_OK) return out;

        // read chunks of max size chunksize from input source
        const int chunkSize = 16384;
        forever
        {

            // read chunk from input and store in stream
            auto source = in.read( chunkSize );
            if( source.isEmpty() )
            {
                out.chop(stream.avail_out);
                break;
            }

            stream.avail_in = source.size();
            stream.next_in = reinterpret_cast<Bytef*>(source.data());

            // uncompress chunk and append to output stream
            int result = Z_OK;
            do
            {

                // prepare destination buffer
                auto size = out.size();
                out.resize( size+chunkSize );
                stream.avail_out = chunkSize;
                stream.next_out = reinterpret_cast<Bytef*>(out.data() + size);

                result = inflate(&stream, Z_NO_FLUSH);
                switch( result )
                {
                    case Z_NEED_DICT:
                    case Z_DATA_ERROR:
                    case Z_STREAM_ERROR:
                    case Z_MEM_ERROR:
                    {
                        inflateEnd( &stream );
                        out.chop(stream.avail_out);
                        return out;
                    }
                }


            } while( stream.avail_out == 0 );

            // Chop off trailing space in the buffer
            out.chop(stream.avail_out);

            if( result == Z_STREAM_END )
            {
                // Make sure there are no more members to process before exiting
                if( !(stream.avail_in && inflateReset(&stream) == Z_OK ) ) break;
            }

        }

        inflateEnd(&stream);
        #endif
        return out;

    }
}
