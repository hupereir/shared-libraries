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

#include "TransparentWidget.h"

#include "BackgroundPixmap.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "Singleton.h"
#include "WinUtil.h"
#include "XmlOptions.h"
#include "X11Util.h"

#include "CompositeEngine.h"

#include <QPainter>

namespace TRANSPARENCY
{

    //____________________________________________________________________
    TransparentWidget::TransparentWidget( QWidget *parent, Qt::WindowFlags flags ):
        QWidget( parent, flags ),
        Counter( "TransparentWidget" ),
        transparent_( false ),
        backgroundChanged_( true ),
        foregroundIntensity_( 255 ),
        shadowOffset_( 0 ),
        highlighted_( false ),
        blurEnabled_( false ),
        opacity_( 1 )
    {

        Debug::Throw( "TransparentWidget::TransparentWidget.\n" );

        /*
        disable all automatic background filling
        to optimize painting.
        */
        setAttribute( Qt::WA_OpaquePaintEvent, true );
        setAttribute( Qt::WA_StyledBackground, false );

        if( TRANSPARENCY::CompositeEngine::get().isAvailable() )
        { setAttribute( Qt::WA_TranslucentBackground, true ); }

        // actions
        _installActions();

        // configuration
        _updateConfiguration();
        connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

        // connections
        connect( &BackgroundPixmap::get(), SIGNAL( backgroundChanged() ), SLOT( setBackgroundChanged() ) );

    }

    //____________________________________________________________________
    void TransparentWidget::setWindowOpacity( double value )
    {

        // store
        opacity_ = value;

        #if defined(Q_OS_WIN)

        // on windows, update opacity only if CompositeEngine is disabled
        // otherwise, it is handled via WinUtil
        if( !TRANSPARENCY::CompositeEngine::get().isEnabled() )
        { QWidget::setWindowOpacity( value ); }

        #else
        QWidget::setWindowOpacity( value );
        #endif

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
    void TransparentWidget::_setHighlightColor( const QColor& color )
    {
        Debug::Throw( "TransparentWidget::highlight.\n" );
        if( highlightColor_ == color ) return;
        highlightColor_ = color;
    }


    //____________________________________________________________________
    void TransparentWidget::moveEvent( QMoveEvent* event )
    {
        Debug::Throw() << "TransparentWidget::moveEvent - " << event->pos().x() << "," << event->pos().y() << endl;

        // check cases where background does not need update
        if( !_transparent() ) return QWidget::moveEvent( event );
        if( CompositeEngine::get().isEnabled() ) return QWidget::moveEvent( event );
        if( _tintColor().isValid() && _tintColor().alpha() == 255 ) return QWidget::moveEvent( event );

        // update background
        setBackgroundChanged( true );
        update();
        QWidget::moveEvent( event );

    }

    //____________________________________________________________________
    void TransparentWidget::resizeEvent( QResizeEvent* event )
    {
        setBackgroundChanged( true );

        #if defined(Q_OS_WIN)
        if( CompositeEngine::get().isEnabled() )
        {
            widgetPixmap_ = QPixmap( size() );
            widgetPixmap_.fill( Qt::transparent );
        }
        #endif

        QWidget::resizeEvent( event );
    }

    //____________________________________________________________________
    void TransparentWidget::showEvent( QShowEvent* event )
    {
        setBackgroundChanged( true );
        update();
        QWidget::showEvent( event );
    }

    //____________________________________________________________________
    void TransparentWidget::enterEvent( QEvent* event )
    {
        if( !_highlighted() && _highlightColor().isValid() )
        {
            _setHighlighted( true );
            update();
        }
        QWidget::enterEvent( event );
    }

    //____________________________________________________________________
    void TransparentWidget::leaveEvent( QEvent* event )
    {
        if( _highlighted() && _highlightColor().isValid() )
        {
            _setHighlighted( false );
            update();
        }
        QWidget::leaveEvent( event );
    }

    //____________________________________________________________________
    void TransparentWidget::paintEvent( QPaintEvent* event )
    {

        Debug::Throw( "TransparentWidget::paintEvent.\n" );

        #if defined(Q_OS_WIN)
        // handle painting on windows with compositing enabled
        // using a pixmap buffer, to allow true transparency
        if( CompositeEngine::get().isEnabled() )
        {
            _paintBackground( widgetPixmap_, event->rect() );
            _paint( widgetPixmap_, event->rect() );
            WinUtil( this ).update( widgetPixmap_, _opacity() );
        } else
            #endif

        {
            _paintBackground( *this, event->rect() );
            _paint( *this, event->rect() );
        }

        Debug::Throw( "TransparentWidget::paintEvent - done.\n" );
    }

    //________________________________________________________________________
    void TransparentWidget::_paintBackground( QPaintDevice& device, const QRect& rect )
    {
        Debug::Throw( "TransparentWidget::_paintBackground.\n" );
        QPainter painter( &device );
        painter.setClipRect( rect );

        if( CompositeEngine::get().isEnabled() )
        {
            painter.setRenderHints(QPainter::SmoothPixmapTransform);
            painter.setCompositionMode(QPainter::CompositionMode_Source );
        }

        if( _backgroundChanged() ) _updateBackgroundPixmap();
        if( !_backgroundPixmap().isNull() )
        { painter.drawPixmap( TransparentWidget::rect(), _backgroundPixmap(), TransparentWidget::rect() ); }

        if( _highlighted() && _highlightColor().isValid() )
        {
            painter.setPen( Qt::NoPen );
            painter.setBrush( _highlightColor() );
            painter.drawRect( TransparentWidget::rect() );
        }

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

        // use transparency
        _setTransparent( XmlOptions::get().get<bool>( "TRANSPARENT" ) );
        _setBlurEnabled( XmlOptions::get().get<bool>( "TRANSPARENCY_USE_BLUR" ) );
        reloadBlurRegionAction_->setEnabled( _blurEnabled() );

        // colors
        QColor color;
        _setForegroundColor( (color = QColor( XmlOptions::get().get<QString>("TRANSPARENCY_FOREGROUND_COLOR"))).isValid() ? color:palette().color( QPalette::WindowText ) );
        _setShadowColor( QColor( XmlOptions::get().get<QString>("TRANSPARENCY_SHADOW_COLOR")) );
        _setForegroundIntensity( XmlOptions::get().get<int>( "TRANSPARENCY_FOREGROUND_INTENSITY" ) );

        // shadow offset
        _setShadowOffset( XmlOptions::get().get<int>("TRANSPARENCY_SHADOW_OFFSET") );

        // inverse colors
        inverseColorsAction().setChecked( XmlOptions::get().get<bool>( "TRANSPARENCY_INVERSE_COLORS" ) );

        // tint
        QColor tintColor( XmlOptions::get().get<QString>( "TRANSPARENCY_TINT_COLOR" ) );
        unsigned int tintIntensity(  XmlOptions::get().get<unsigned int>( "TRANSPARENCY_TINT_INTENSITY" ) );
        if( tintColor.isValid() && tintIntensity )
        {

            tintColor.setAlpha( tintIntensity );
            _setTintColor( tintColor );

        } else _setTintColor( QColor() );

        // highlight
        QColor highlightColor( XmlOptions::get().get<QString>( "TRANSPARENCY_HIGHLIGHT_COLOR" ) );
        unsigned int highlightIntensity(  XmlOptions::get().get<unsigned int>( "TRANSPARENCY_HIGHLIGHT_INTENSITY" ) );
        if( highlightColor.isValid() && highlightIntensity )
        {
            highlightColor.setAlpha( highlightIntensity );
            _setHighlightColor( highlightColor );
        } else _setHighlightColor( QColor() );

        // composite
        if( CompositeEngine::get().setEnabled( XmlOptions::get().get<bool>( "TRANSPARENCY_USE_COMPOSITE" ) ) )
        { setWindowOpacity( _opacity() ); }

        #if defined(Q_OS_WIN)
        // create widget pixmap when compositing is enabled
        if( CompositeEngine::get().isEnabled() ) {
            widgetPixmap_ = QPixmap( size() );
            widgetPixmap_.fill( Qt::transparent );
        } else widgetPixmap_ = QPixmap();
        #endif

    }

    //____________________________________________________________________
    void TransparentWidget::_reloadBackground( void )
    {
        Debug::Throw( "TransparentWidget::_reloadBackground.\n" );
        BackgroundPixmap::get().reload();
    }

    //____________________________________________________________________
    void TransparentWidget::_updateBackgroundPixmap( void )
    {

        Debug::Throw( "TransparentWidget::_updateBackgroundPixmap.\n" );

        if( ( !_transparent() ) ||  ( _tintColor().isValid() && _tintColor().alpha() == 255 ) )
        {

            // solid background
            _backgroundPixmap() = QPixmap( size() );
            _backgroundPixmap().fill( palette().color( backgroundRole() ) );

        } else if( CompositeEngine::get().isEnabled() ) {

            _backgroundPixmap() = QPixmap( size() );
            QPainter painter( &_backgroundPixmap() );
            painter.setRenderHints(QPainter::SmoothPixmapTransform);
            painter.setCompositionMode(QPainter::CompositionMode_Source );
            painter.fillRect( _backgroundPixmap().rect(), Qt::transparent);

        } else {

            // transparent background
            _backgroundPixmap() = BackgroundPixmap::get().pixmap( QRect( mapToGlobal( QPoint(0,0) ), size() ) );
            if( _backgroundPixmap().isNull() )
            {
                _backgroundPixmap() = QPixmap( size() );
                _backgroundPixmap().fill( palette().color( backgroundRole() ) );
            }

        }

        // tint
        if( _tintColor().isValid() )
        {

            QPainter painter( &_backgroundPixmap() );
            painter.setPen( Qt::NoPen );
            painter.setBrush( _tintColor() );
            painter.drawRect( _backgroundPixmap().rect() );
            painter.end();
        }

        setBackgroundChanged( false );

        return;

    }


    //____________________________________________________________________
    void TransparentWidget::_installActions( void )
    {
        Debug::Throw( "TransparentWidget::_installAction.\n" );

        addAction( updateBackgroundAction_ = new QAction( IconEngine::get( ICONS::RELOAD ), "Update Background", this ) );
        connect( updateBackgroundAction_, SIGNAL( triggered() ), SLOT( _updateBackgroundPixmap() ) );

        reloadBackgroundAction_ = new QAction( IconEngine::get( ICONS::RELOAD ), "Reload Background", this );
        reloadBackgroundAction_->setToolTip( "Reinitialize transparent background" );
        reloadBackgroundAction_->setShortcut( QKeySequence::Refresh );
        connect( reloadBackgroundAction_, SIGNAL( triggered( void ) ), SLOT( _reloadBackground( void ) ) );

        addAction( reloadBlurRegionAction_ = new QAction( IconEngine::get( ICONS::RELOAD ), "Reload Blur Region", this ) );
        connect( reloadBlurRegionAction_, SIGNAL( triggered( void ) ), SLOT( _updateBlurRegion( void ) ) );

        addAction( inverseColorsAction_ = new QAction( "Invert Colors", this ) );
        inverseColorsAction_->setCheckable( true );
        inverseColorsAction_->setChecked( false );
        connect( inverseColorsAction_, SIGNAL( toggled( bool ) ), SLOT( _toggleInverseColors( bool ) ) );

    }


    //__________________________________________________________
    void TransparentWidget::_updateBlurRegion( const QRegion& region )
    {

        Debug::Throw() << "TransparentWidget::_updateBlurRegion" << endl;
        blurRegion_ = region;
        QVector<QRect> rects( region.rects() );
        QVector<unsigned long> data;
        foreach( const QRect& r, rects )
        { data << r.x() << r.y() << r.width() << r.height(); }

        X11Util::get().changeProperty(
            *this,
            X11Util::_KDE_NET_WM_BLUR_BEHIND_REGION,
            reinterpret_cast<const unsigned char *>(data.constData()),
            data.size());

    }

}
