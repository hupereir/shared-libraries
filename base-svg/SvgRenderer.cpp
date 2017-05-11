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

#include "SvgRenderer.h"
#include "Svg.h"
#include "XmlOptions.h"

#include <QPainter>
#include <QDomDocument>

#if WITH_ZLIB
#include <zlib.h>
#endif

namespace Svg
{

    //________________________________________________
    SvgRenderer::SvgRenderer( void ):
        QSvgRenderer(),
        Counter( "Svg::SvgRendered" )
    {}


    //________________________________________________
    bool SvgRenderer::updateConfiguration( void )
    {
        bool drawOverlay( XmlOptions::get().get<bool>( "SVG_DRAW_OVERLAY" ) );
        if( drawOverlay == drawOverlay_ ) return false;
        drawOverlay_ = drawOverlay;
        return true;
    }

    //________________________________________________
    void SvgRenderer::createStyleSheet( QPalette palette )
    {
        palette.setCurrentColorGroup( QPalette::Active );
        styleSheet_.clear();

        auto addColor = [](QString name, QColor color )
        { return QString( ".ColorScheme-%1 {\n  color:%2;\n  stop-color:%2;\n}\n" ).arg( name, color.name()); };

        styleSheet_ += addColor( "Text", palette.color( QPalette::WindowText ) );
        styleSheet_ += addColor( "Background", palette.color( QPalette::Window ) );
        styleSheet_ += addColor( "Highlight", palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( "ViewText", palette.color( QPalette::Text ) );
        styleSheet_ += addColor( "ViewBackground", palette.color( QPalette::Base ) );
        styleSheet_ += addColor( "ViewHover", palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( "ViewFocus", palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( "ButtonText", palette.color( QPalette::ButtonText ) );
        styleSheet_ += addColor( "ButtonBackground", palette.color( QPalette::Button ) );
        styleSheet_ += addColor( "ButtonHover", palette.color( QPalette::Highlight ) );
        styleSheet_ += addColor( "ButtonFocus", palette.color( QPalette::Highlight ) );
    }

    //________________________________________________
    bool SvgRenderer::load( QString filename )
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
            QDomElement element = document.createElement( QLatin1String( "style" ) );
            element.setAttribute( QLatin1String( "type" ), QLatin1String( "text/css" ) );
            element.setAttribute( QLatin1String( "id" ), QLatin1String( "current-color-scheme" ) );
            element.appendChild( document.createTextNode( styleSheet_ ) );

            // find child in document
            auto&& children( document.elementsByTagName( element.tagName() ) );
            bool replaced( false );
            for( int i = 0; i < children.size() && !replaced; ++i )
            {

                auto&& child( children.at(i) );
                if( child.toElement().attribute( QLatin1String( "id" ) ) == QLatin1String( "current-color-scheme" ) )
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

        if( loaded )
        {
            isValid_ = _hasPrefix();
            if( !_hasMargins() )
            {
                hasShadowMargins_ = _hasMargins( "shadow" );
                hasShadowPrefix_ = _hasPrefix( "shadow", Ring );
                hasShadow_ = hasShadowMargins_ && hasShadowPrefix_;
            }

            hasOverlay_ = elementExists( Svg::Overlay );

            // centering hints
            overlayHints_ = OverlayNone;
            if( elementExists( "hint-overlay-pos-right" ) ) overlayHints_ |= OverlayPosRight;
            if( elementExists( "hint-overlay-pos-bottom" ) ) overlayHints_ |= OverlayPosBottom;

            if( elementExists( "hint-overlay-stretch") ) overlayHints_ |= OverlayStretch;
            else if( elementExists( "hint-overlay-tile-horizontal" ) ) overlayHints_ |= OverlayTileHorizontal;
            else if( elementExists( "hint-overlay-tile-vertical") ) overlayHints_ |= OverlayTileVertical;

            hints_ = HintNone;
            if( elementExists( "hint-compose-over-border" ) ) hints_ = HintComposeOverBorder;

            if( _hasPrefix( "mask-overlay" ) ) maskPrefix_ = "mask-overlay";
            else if( _hasPrefix( "mask" ) ) maskPrefix_ = "mask";
            else maskPrefix_ = QString();

        } else {

            isValid_ = false;

        }

        return loaded;

    }

    //________________________________________________
    void SvgRenderer::render( QPaintDevice& device, QString id )
    {

        // check device size
        if( device.height() <= 0 || device.width() <= 0 ) return;

        // create painter
        if( !id.isEmpty() )
        {

            double width = device.width();
            double height = device.height();

            QPainter painter( &device );
            QSvgRenderer::render( &painter, id, QRectF( QPointF( 0, 0 ), QSizeF( width, height ) ) );

        } else {


            if( hasShadow_ )
            {

                // create painter
                QPainter painter( &device );

                // shadow image
                QSize size( device.width(), device.height() );
                QImage shadowImage( size, QImage::Format_ARGB32_Premultiplied );
                shadowImage.fill( Qt::transparent );

                _render( shadowImage, "shadow", Ring );

                painter.drawImage( QPoint(0,0), shadowImage );

                // main image
                const Base::Margins outerPadding( this->outerPadding() );
                size.rwidth() -= outerPadding.width();
                size.rheight() -= outerPadding.height();
                QImage mainImage( size, QImage::Format_ARGB32_Premultiplied );
                mainImage.fill( Qt::transparent );

                _renderPanel( mainImage );
                painter.drawImage( outerPadding.topLeft(), mainImage );

            } else _renderPanel( device );

        }

        return;

    }

    //________________________________________________
    QByteArray SvgRenderer::_tryUncompress( QIODevice& in ) const
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

    //________________________________________________
    bool SvgRenderer::_hasPrefix( QString prefix, SvgElements mask ) const
    {

        if( !prefix.isEmpty() ) prefix+="-";

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( (mask & TopLeft) && !elementExists( prefix+Svg::TopLeft ) ) return false;
        if( (mask & Top) && !elementExists( prefix+Svg::Top ) ) return false;
        if( (mask & TopRight) && !elementExists( prefix+Svg::TopRight ) ) return false;

        if( (mask & BottomLeft) && !elementExists( prefix+Svg::BottomLeft ) ) return false;
        if( (mask & Bottom) && !elementExists( prefix+Svg::Bottom ) ) return false;
        if( (mask & BottomRight) && !elementExists( prefix+Svg::BottomRight ) ) return false;

        if( (mask & Left) && !elementExists( prefix+Svg::Left ) ) return false;
        if( (mask & Center) && !elementExists( prefix+Svg::Center ) ) return false;
        if( (mask & Right) && !elementExists( prefix+Svg::Right ) ) return false;
        return true;

    }


    //________________________________________________
    bool SvgRenderer::_hasMargins( QString prefix, SvgElements mask ) const
    {

        if( !prefix.isEmpty() ) prefix+="-";

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( (mask & Top) && !elementExists( prefix + Svg::MarginTop ) ) return false;
        if( (mask & Left) && !elementExists( prefix + Svg::MarginLeft ) ) return false;
        if( (mask & Right) && !elementExists( prefix + Svg::MarginRight ) ) return false;
        if( (mask & Bottom) && !elementExists( prefix + Svg::MarginBottom ) ) return false;
        return true;

    }

    //________________________________________________
    Base::Margins SvgRenderer::margins( void ) const
    {
        Base::Margins out;
        if( elementExists( Svg::MarginLeft ) ) out.setLeft( boundsOnElement( Svg::MarginLeft ).width() );
        if( elementExists( Svg::MarginRight ) ) out.setRight( boundsOnElement( Svg::MarginRight ).width() );
        if( elementExists( Svg::MarginTop ) ) out.setTop( boundsOnElement( Svg::MarginTop ).height() );
        if( elementExists( Svg::MarginBottom ) ) out.setBottom( boundsOnElement( Svg::MarginBottom ).height() );

        // add shadow size if available
        if( hasShadow_ ) out += outerPadding();

        return out;
    }

    //________________________________________________
    Base::Margins SvgRenderer::outerPadding( void ) const
    {
        Base::Margins out;
        if( hasShadowMargins_ )
        {
            out.setLeft( boundsOnElement( "shadow-"+Svg::MarginLeft ).width() );
            out.setRight( boundsOnElement( "shadow-"+Svg::MarginRight ).width() );
            out.setTop( boundsOnElement( "shadow-"+Svg::MarginTop ).height() );
            out.setBottom( boundsOnElement( "shadow-"+Svg::MarginBottom ).height() );

        } else {

            if( elementExists( Svg::ShadowLeft ) ) out.setLeft( boundsOnElement( Svg::ShadowLeft ).size().width() );
            if( elementExists( Svg::ShadowRight ) ) out.setRight( boundsOnElement( Svg::ShadowRight ).size().width() );
            if( elementExists( Svg::ShadowTop ) ) out.setTop( boundsOnElement( Svg::ShadowTop ).size().height() );
            if( elementExists( Svg::ShadowBottom ) ) out.setBottom( boundsOnElement( Svg::ShadowBottom ).size().height() );

        }

        return out;

    }

    //________________________________________________
    void SvgRenderer::_renderPanel( QPaintDevice& device )
    {

        // create painter
        QPainter painter( &device );

        // size
        QSize size( device.width(), device.height() );

        //Overlays
        if( drawOverlay_ && hasOverlay_ )
        {

            // compute painting rect
            QSizeF overlaySize = boundsOnElement(Svg::Overlay).size();

            // size hints
            if( overlayHints_ & OverlayStretch ) overlaySize = size;
            else if( overlayHints_ & OverlayTileHorizontal ) overlaySize.setWidth( size.width() );
            else if( overlayHints_ & OverlayTileVertical ) overlaySize.setHeight( size.height() );
            QRectF overlayRect( QPointF(0,0), overlaySize );

            // create image and paint
            QImage overlayImage( QSize( device.width(), device.height() ),  QImage::Format_ARGB32_Premultiplied );

            overlayImage.fill( Qt::transparent );
            QPainter overlayPainter( &overlayImage );

            // centering hints
            if( overlayHints_ & OverlayPosRight ) overlayPainter.translate( device.width() - overlayRect.width(), 0 );
            if( overlayHints_ & OverlayPosBottom ) overlayPainter.translate( 0, device.height() - overlayRect.height() );

            QSvgRenderer::render( &overlayPainter, Svg::Overlay, overlayRect );
            overlayPainter.end();

            // draw on main painter
            painter.drawImage( QPointF(0,0), overlayImage );

        }

        // center
        if( hints_ & HintComposeOverBorder )
        {
            QImage centerImage( size, QImage::Format_ARGB32_Premultiplied );
            centerImage.fill( Qt::transparent );
            _render( centerImage, "", Center, false );
            painter.drawImage( QPoint(0,0), centerImage );
        }

        // set mask
        if( !maskPrefix_.isEmpty() && ((hints_ & HintComposeOverBorder) || (drawOverlay_ && hasOverlay_ )) )
        {
            // create mask
            QImage maskImage( size, QImage::Format_ARGB32_Premultiplied );
            maskImage.fill( Qt::transparent );
            _render( maskImage, maskPrefix_ );

            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            painter.drawImage( QPointF(0,0), maskImage );

            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        }

        // main image
        QImage mainImage( size, QImage::Format_ARGB32_Premultiplied );
        mainImage.fill( Qt::transparent );

        //
        if( hints_ & HintComposeOverBorder ) _render( mainImage, "", Ring );
        else  _render( mainImage );

        painter.drawImage( QPoint(0,0), mainImage );

    }

    //________________________________________________
    void SvgRenderer::_render( QImage& target, QString prefix, SvgElements elements, bool padding  )
    {

        // check target
        if( target.isNull() ) return;

        const QRect targetRect( target.rect() );
        if( !targetRect.isValid() ) return;

        // check prefix
        if( !prefix.isEmpty() ) prefix += "-";

        // paint svg onto image
        QPainter painter( &target );

        QRectF centerRect( targetRect );
        if( padding )
        {
            centerRect.adjust(
                boundsOnElement( prefix+Svg::Left ).width(),
                boundsOnElement( prefix+Svg::Top ).height(),
                -boundsOnElement( prefix+Svg::Right ).width(),
                -boundsOnElement( prefix+Svg::Bottom ).height() );
        }

        // center
        if( centerRect.isValid() && elements & Center )
        { QSvgRenderer::render( &painter, prefix+Svg::Center, centerRect ); }

        if( padding )
        {

            if( elements & TopLeft )
            {
                // topLeft corner
                painter.setClipRect( QRect( targetRect.topLeft(), targetRect.center() ) );
                QSvgRenderer::render( &painter, prefix+Svg::TopLeft, QRectF( QPointF( 0, 0 ), boundsOnElement( prefix+Svg::TopLeft ).size() ) );
            }

            if( elements & TopRight )
            {
                // topRight corner
                painter.setClipRect( QRect( QPoint(  targetRect.center().x()+1, targetRect.top() ), QPoint( targetRect.right(), targetRect.center().y() ) ) );
                QSvgRenderer::render( &painter, prefix+Svg::TopRight, QRectF( QPointF( centerRect.right(), 0 ), boundsOnElement( prefix+Svg::TopRight ).size() ) );
            }

            if( elements & BottomLeft )
            {

                // topRight corner
                painter.setClipRect( QRect( QPoint(  targetRect.left(), targetRect.center().y()+1 ), QPoint( targetRect.center().x(), targetRect.bottom() ) ) );
                QSvgRenderer::render( &painter, prefix+Svg::BottomLeft, QRectF( QPointF( 0, centerRect.bottom() ), boundsOnElement( prefix+Svg::BottomRight ).size() ) );
            }

            if( elements & BottomRight )
            {
                // bottomRight corner
                painter.setClipRect( QRect( targetRect.center()+QPoint( 1, 1 ), targetRect.bottomRight() ) );
                QSvgRenderer::render( &painter, prefix+Svg::BottomRight, QRectF( centerRect.bottomRight(), boundsOnElement( prefix+Svg::BottomRight ).size() ) );
            }

            if( centerRect.width() > 0 )
            {

                if( elements & Top )
                {
                    // top size
                    painter.setClipRect( QRect( targetRect.topLeft(), QPoint( targetRect.right(), targetRect.center().y() ) ) );
                    QSvgRenderer::render( &painter, prefix+Svg::Top, QRectF( QPointF( centerRect.left(), 0 ), QSizeF( centerRect.width(), boundsOnElement( prefix+Svg::Top ).height() ) ) );
                }

                if( elements & Bottom )
                {
                    // bottom size
                    painter.setClipRect( QRect( QPoint( targetRect.left(), targetRect.center().y()+1 ), targetRect.bottomRight() ) );
                    QSvgRenderer::render( &painter, prefix+Svg::Bottom, QRectF( centerRect.bottomLeft(), QSizeF( centerRect.width(), boundsOnElement( prefix+Svg::Bottom ).height() ) ) );
                }

            }

            if( centerRect.height() > 0 )
            {

                if( elements & Left )
                {
                    // left side
                    painter.setClipRect( QRect( targetRect.topLeft(), QPoint( targetRect.center().x(), targetRect.bottom() ) ) );
                    QSvgRenderer::render( &painter, prefix+Svg::Left, QRectF( QPointF( 0, centerRect.top() ), QSizeF( boundsOnElement( prefix+Svg::Left ).width(), centerRect.height() ) ) );
                }

                if( elements & Right )
                {
                    painter.setClipRect( QRect( QPoint( targetRect.center().x()+1, targetRect.top() ), targetRect.bottomRight() ) );
                    QSvgRenderer::render( &painter, prefix+Svg::Right, QRectF( centerRect.topRight(), QSizeF( boundsOnElement( prefix+Svg::Right ).width(), centerRect.height() ) ) );
                }

            }

        }

        painter.end();
        return;
    }

}
