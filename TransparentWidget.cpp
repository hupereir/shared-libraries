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
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "TransparentWidget.h"
#include "TransparentWidget.moc"

#include "BaseIconNames.h"
#include "Color.h"
#include "CompositeEngine.h"
#include "IconEngine.h"
#include "Singleton.h"
#include "WinUtil.h"
#include "XmlOptions.h"
#include "XcbUtil.h"

#include <QPainter>

#if HAVE_X11
#include <xcb/xcb.h>
#include <xcb/shape.h>
#endif

namespace Transparency
{

    //____________________________________________________________________
    TransparentWidget::TransparentWidget( QWidget *parent, Qt::WindowFlags flags ):
        QWidget( parent, flags ),
        Counter( "Transparency::TransparentWidget" ),
        hasInputShape_( false ),
        backgroundChanged_( true ),
        foregroundIntensity_( 255 ),
        shadowOffset_( 0 )
    {

        Debug::Throw( "TransparentWidget::TransparentWidget.\n" );

        /*
        disable all automatic background filling
        to optimize painting.
        */
        setAttribute( Qt::WA_OpaquePaintEvent, true );
        setAttribute( Qt::WA_StyledBackground, false );

        if( Transparency::CompositeEngine::get().isAvailable() )
        { setAttribute( Qt::WA_TranslucentBackground, true ); }

        // actions
        _installActions();

        // configuration
        _updateConfiguration();

        if( Singleton::get().hasApplication() )
        { connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) ); }

    }

    //____________________________________________________________________
    void TransparentWidget::setBackgroundChanged( void )
    {
        Debug::Throw( "TransparentWidget::setBackgroundChanged.\n"  );
        setBackgroundChanged( true );
        update();
    }

    //____________________________________________________________________
    void TransparentWidget::_setForegroundColor( const QColor& color )
    {
        if( !color.isValid() ) return;
        foregroundColor_ = color;
        foregroundColor_.setAlpha( foregroundIntensity_ );
    }

    //____________________________________________________________________
    void TransparentWidget::_setShadowColor( const QColor& color )
    {
        if( !color.isValid() ) return;
        shadowColor_ = color;
        shadowColor_.setAlpha( foregroundIntensity_ );
    }


    //____________________________________________________________________
    void TransparentWidget::_setForegroundIntensity( int value )
    {
        foregroundIntensity_ = value;
        if( foregroundColor_.isValid() )  foregroundColor_.setAlpha( foregroundIntensity_ );
        if( shadowColor_.isValid() )  shadowColor_.setAlpha( foregroundIntensity_ );
    }

    //____________________________________________________________________
    void TransparentWidget::_setTintColor( const QColor& color )
    {
        Debug::Throw( "TransparentWidget::_setTintColor.\n" );
        if( tintColor_ == color ) return;
        tintColor_ = color;
        setBackgroundChanged( true );
    }

    //____________________________________________________________________
    void TransparentWidget::resizeEvent( QResizeEvent* event )
    {
        setBackgroundChanged( true );

        #if defined(Q_OS_WIN)
        if( CompositeEngine::get().isAvailable() )
        {
            widgetPixmap_ = QPixmap( size() );
            widgetPixmap_.fill( Qt::transparent );
        }
        #endif

        QWidget::resizeEvent( event );
        _updateInputShape();

    }

    //____________________________________________________________________
    void TransparentWidget::showEvent( QShowEvent* event )
    {
        setBackgroundChanged( true );
        update();
        QWidget::showEvent( event );
        _updateInputShape();
    }

    //____________________________________________________________________
    void TransparentWidget::paintEvent( QPaintEvent* event )
    {

        Debug::Throw( "TransparentWidget::paintEvent.\n" );

        #if defined(Q_OS_WIN)
        _paintBackground( widgetPixmap_, event->rect() );
        _paint( widgetPixmap_, event->rect() );
        WinUtil( this ).update( widgetPixmap_ );
        #else
        _paintBackground( *this, event->rect() );
        _paint( *this, event->rect() );
        #endif

    }

    //________________________________________________________________________
    void TransparentWidget::_paintBackground( QPaintDevice& device, const QRect& rect )
    {
        Debug::Throw( "TransparentWidget::_paintBackground.\n" );
        QPainter painter( &device );
        painter.setClipRect( rect );

        if( CompositeEngine::get().isAvailable() )
        {
            painter.setRenderHints(QPainter::SmoothPixmapTransform);
            painter.setCompositionMode(QPainter::CompositionMode_Source );
        }

        if( backgroundChanged_ ) _updateBackgroundPixmap();
        if( !backgroundPixmap_.isNull() )
        { painter.drawPixmap( TransparentWidget::rect(), backgroundPixmap_, TransparentWidget::rect() ); }

        painter.end();

    }

    //____________________________________________________________________
    void TransparentWidget::_toggleInverseColors( bool value )
    {
        XmlOptions::get().set<bool>( "TRANSPARENCY_INVERSE_COLORS", value );
        update();
    }

    //____________________________________________________________________
    void TransparentWidget::_updateConfiguration( void )
    {

        Debug::Throw( "TransparentWidget::_updateConfiguration.\n" );

        // colors
        QColor color;
        _setForegroundColor( (color = QColor( XmlOptions::get().get<Base::Color>("TRANSPARENCY_FOREGROUND_COLOR"))).isValid() ? color:palette().color( QPalette::WindowText ) );
        _setShadowColor( QColor( XmlOptions::get().get<Base::Color>("TRANSPARENCY_SHADOW_COLOR")) );
        _setForegroundIntensity( XmlOptions::get().get<int>( "TRANSPARENCY_FOREGROUND_INTENSITY" ) );

        // shadow offset
        _setShadowOffset( XmlOptions::get().get<int>("TRANSPARENCY_SHADOW_OFFSET") );

        // inverse colors
        inverseColorsAction().setChecked( XmlOptions::get().get<bool>( "TRANSPARENCY_INVERSE_COLORS" ) );

        // tint
        QColor tintColor( XmlOptions::get().get<Base::Color>( "TRANSPARENCY_TINT_COLOR" ) );
        unsigned int tintIntensity(  XmlOptions::get().get<int>( "TRANSPARENCY_TINT_INTENSITY" ) );
        if( tintColor.isValid() && tintIntensity )
        {

            tintColor.setAlpha( tintIntensity );
            _setTintColor( tintColor );

        } else _setTintColor( QColor() );

        #if defined(Q_OS_WIN)
        // create widget pixmap when compositing is enabled
        if( CompositeEngine::get().isAvailable() )
        {

            widgetPixmap_ = QPixmap( size() );
            widgetPixmap_.fill( Qt::transparent );

        } else widgetPixmap_ = QPixmap();
        #endif

        // update input shape
        if( testAttribute(Qt::WA_WState_Created) || internalWinId() )
        { _updateInputShape(); }

    }

    //____________________________________________________________________
    void TransparentWidget::_updateBackgroundPixmap( void )
    {

        Debug::Throw( "TransparentWidget::_updateBackgroundPixmap.\n" );

        backgroundPixmap_ = QPixmap( size() );
        if( CompositeEngine::get().isAvailable() ) backgroundPixmap_.fill( Qt::transparent );
        else if( !( tintColor_.isValid() && tintColor_.alpha() == 255 ) ) backgroundPixmap_.fill( palette().color( backgroundRole() ) );

        // tint
        if( tintColor_.isValid() ) backgroundPixmap_.fill( tintColor_ );

        setBackgroundChanged( false );

        return;

    }


    //____________________________________________________________________
    void TransparentWidget::_installActions( void )
    {
        Debug::Throw( "TransparentWidget::_installAction.\n" );

        addAction( reloadBlurRegionAction_ = new QAction( IconEngine::get( IconNames::Reload ), tr( "Reload Blur Region" ), this ) );
        connect( reloadBlurRegionAction_, SIGNAL(triggered()), SLOT(_updateBlurRegion()) );

        addAction( inverseColorsAction_ = new QAction( tr( "Inverse Colors" ), this ) );
        inverseColorsAction_->setCheckable( true );
        inverseColorsAction_->setChecked( false );
        connect( inverseColorsAction_, SIGNAL(toggled(bool)), SLOT(_toggleInverseColors(bool)) );

    }

    //________________________________________________
    void TransparentWidget::_updateInputShape( void )
    {

        #if HAVE_X11

        // check outer padding
        // TODO: should remove existing shape if any
        // TODO: should check shape extension version
        if( _outerPadding().isNull() )
        {

            if( hasInputShape_ )
            {
                // reset shape
                xcb_connection_t* connection( reinterpret_cast<xcb_connection_t*>( XcbUtil::get().connection() ) );
                xcb_shape_mask( connection, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, winId(), 0, 0, XCB_PIXMAP_NONE);
                xcb_flush( connection );

                hasInputShape_ = false;
            }

        } else {

            // update shape
            const QRect rect( _outerPadding().adjustedRect( this->rect() ) );
            xcb_rectangle_t xrect = { rect.x(), rect.y(), rect.width(), rect.height() };

            xcb_connection_t* connection( reinterpret_cast<xcb_connection_t*>( XcbUtil::get().connection() ) );
            xcb_shape_rectangles(
                connection,
                XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, XCB_CLIP_ORDERING_YX_BANDED, winId(),
                0, 0, 1, &xrect );

            xcb_flush( connection );

            hasInputShape_ = true;

        }

        #endif

    }

    //__________________________________________________________
    void TransparentWidget::_updateBlurRegion( const QRegion& region )
    {

        Debug::Throw() << "TransparentWidget::_updateBlurRegion" << endl;

        #if HAVE_X11

        // create data
        blurRegion_ = region;
        QVector<QRect> rects( region.rects() );
        QVector<uint32_t> data;
        foreach( const QRect& r, rects )
        { data << r.x() << r.y() << r.width() << r.height(); }

        // get connection and atom
        xcb_connection_t* connection( reinterpret_cast<xcb_connection_t*>( XcbUtil::get().connection() ) );
        xcb_atom_t atom( static_cast<xcb_atom_t>( XcbUtil::get().findAtom( X11Defines::_KDE_NET_WM_BLUR_BEHIND_REGION ) ) );
        xcb_change_property( connection, XCB_PROP_MODE_REPLACE, winId(), atom, XCB_ATOM_CARDINAL, 32, data.size(), data.constData() );
        xcb_flush( connection );

        #endif

    }

}
