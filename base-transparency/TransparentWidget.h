#ifndef TransparentWidget_h
#define TransparentWidget_h

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

#include "base_transparency_export.h"
#include "Counter.h"
#include "Margins.h"

#include <QAction>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

namespace Transparency
{

    //* transparent widget
    class BASE_TRANSPARENCY_EXPORT TransparentWidget: public QWidget, private Base::Counter<TransparentWidget>
    {

        //* Qt meta object declaration
        Q_OBJECT

        public:

        //* constructor
        explicit TransparentWidget( QWidget* = nullptr, Qt::WindowFlags flags = 0 );

        //*@name actions
        //@{

        //* reload blur region action
        QAction& reloadBlurRegionAction() const
        { return *reloadBlurRegionAction_; }

        //* inverse colors
        QAction& inverseColorsAction() const
        { return *inverseColorsAction_; }

        //@}

        //*@name accessors
        //@{

        //* foreground
        virtual const QColor& foregroundColor() const
        { return inverseColorsAction().isChecked() ? shadowColor_ : foregroundColor_; }

        //* shadow
        virtual const QColor& shadowColor() const
        { return inverseColorsAction().isChecked() ? foregroundColor_ : shadowColor_; }

        //* foreground intensity
        virtual int foregroundIntensity() const
        { return foregroundIntensity_; }

        //* shadow offset
        virtual int shadowOffset() const
        { return shadowOffset_; }

        //@}

        //*@name modifiers
        //@{

        //* background changed
        void setBackgroundChanged( bool value )
        { backgroundChanged_ = value; }

        //* force reloading of the background
        virtual void setBackgroundChanged();

        //@}

        protected:

        //* device pixel ratio
        virtual void _setDevicePixelRatio( qreal ratio )
        { devicePixelRatio_ = ratio; }
        
        //* foreground
        virtual void _setForegroundColor( const QColor& );

        //* shadow
        virtual void _setShadowColor( const QColor& );

        //* tint
        virtual void _setTintColor( const QColor& );

        //* tint color
        virtual const QColor& _tintColor() const
        { return tintColor_; }

        //* shadow offset
        virtual void _setShadowOffset( int value )
        { shadowOffset_ = value; }

        //* foreground intensity
        virtual void _setForegroundIntensity( int value );

        //* background pixmap
        virtual QPixmap& _backgroundPixmap()
        { return backgroundPixmap_; }

        //* background pixmap
        virtual const QPixmap& _backgroundPixmap() const
        { return backgroundPixmap_; }

        //*@name event handlers
        //@{

        //* resize
        void resizeEvent( QResizeEvent* ) override;

        //* show
        void showEvent( QShowEvent* ) override;

        //* paint
        void paintEvent( QPaintEvent* ) override;

        //@}

        //*@name margins
        //@{

        //* margins
        const Base::Margins& _margins() const
        { return margins_; }

        //* clear margins
        void _clearMargins()
        { margins_.clear(); }

        //* margins
        void _setMargins( Base::Margins margins )
        { margins_ = margins; }

        //* margins
        void _setMargins( int margins )
        { margins_ = Base::Margins( margins ); }

        //* outer padding
        Base::Margins _outerPadding() const
        { return outerPadding_; }

        //* clear outer padding
        void _clearOuterPadding()
        { outerPadding_.clear(); }

        //* outer padding
        void _setOuterPadding( Base::Margins margins )
        { outerPadding_ = margins; }

        //* outer padding
        void _setOuterPadding( int margins )
        { outerPadding_ = Base::Margins( margins ); }

        //@}

        //* background changed
        bool _backgroundChanged() const
        { return backgroundChanged_; }

        //* paint background on devide
        virtual void _paintBackground( QPaintDevice&, QRect );

        //* paint main widget on devide
        /*! this must be re-implemented by derived classes */
        virtual void _paint( QPaintDevice&, QRect )
        {}

        //* update input shape (to prevent input events in outerPadding region)
        virtual void _updateInputShape();

        //* update blur region
        virtual void _updateBlurRegion( const QRegion& );

        //* update background pixmap
        virtual void _updateBackgroundPixmap();

        //* update blur region
        virtual void _updateBlurRegion()
        {
            if( blurRegion_.isEmpty() ) blurRegion_ = rect().translated( mapTo( window(), QPoint(0,0) ) );
            _updateBlurRegion( blurRegion_ );
        }

        //* toggle inverse colors
        virtual void _toggleInverseColors( bool );

        private:

        //* update configuration
        void _updateConfiguration();

        //* actions
        void _installActions();

        //*@name actions
        //@{

        //* reload blur region
        QAction* reloadBlurRegionAction_ = nullptr;

        //* inverse colors
        QAction* inverseColorsAction_ = nullptr;

        //@}

        //* true if input shape is set
        bool hasInputShape_ = false;

        //* true when background needs to be reloaded
        bool backgroundChanged_ = true;

        //* foreground
        QColor foregroundColor_;

        //* shadow
        QColor shadowColor_;

        //* tint color
        QColor tintColor_;

        //* device pixel ratio
        qreal devicePixelRatio_ = 1;
        
        //* foreground intensity
        int foregroundIntensity_ = 255;

        //* shadow offset
        int shadowOffset_ = 0;

        //* margins
        Base::Margins margins_;

        //* margins
        Base::Margins outerPadding_;

        //* store last blur region
        QRegion blurRegion_;

        #if defined(Q_OS_WIN)
        //* widget pixmap
        /*! it is used as widget storage when using full translucency */
        QPixmap widgetPixmap_;
        #endif

        //* background pixmap
        QPixmap backgroundPixmap_;

    };

};

#endif
