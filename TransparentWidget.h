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

#include "Counter.h"
#include "Margins.h"

#include <QAction>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

namespace Transparency
{

    //! transparent widget
    class TransparentWidget: public QWidget, public Counter
    {

        //! Qt meta object declaration
        Q_OBJECT

        public:

        //! constructor
        TransparentWidget( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

        //!@name actions
        //@{

        //! reload blur region action
        QAction& reloadBlurRegionAction( void ) const
        { return *reloadBlurRegionAction_; }

        //! inverse colors
        QAction& inverseColorsAction( void ) const
        { return *inverseColorsAction_; }

        //@}

        //!@name accessors
        //@{

        //! foreground
        virtual const QColor& foregroundColor( void ) const
        { return inverseColorsAction().isChecked() ? shadowColor_ : foregroundColor_; }

        //! shadow
        virtual const QColor& shadowColor( void ) const
        { return inverseColorsAction().isChecked() ? foregroundColor_ : shadowColor_; }

        //! foreground intensity
        virtual int foregroundIntensity( void ) const
        { return foregroundIntensity_; }

        //! shadow offset
        virtual int shadowOffset( void ) const
        { return shadowOffset_; }

        //@}

        //!@name modifiers
        //@{

        //! background changed
        void setBackgroundChanged( bool value )
        { backgroundChanged_ = value; }

        //@}

        public Q_SLOTS:

        //! force reloading of the background
        virtual void setBackgroundChanged( void );

        protected:

        //! foreground
        virtual void _setForegroundColor( const QColor& );

        //! shadow
        virtual void _setShadowColor( const QColor& );

        //! tint
        virtual void _setTintColor( const QColor& );

        //! tint color
        virtual const QColor& _tintColor( void ) const
        { return tintColor_; }

        //! shadow offset
        virtual void _setShadowOffset( int value )
        { shadowOffset_ = value; }

        //! foreground intensity
        virtual void _setForegroundIntensity( int value );

        //! background pixmap
        virtual QPixmap& _backgroundPixmap( void )
        { return backgroundPixmap_; }

        //! background pixmap
        virtual const QPixmap& _backgroundPixmap( void ) const
        { return backgroundPixmap_; }

        //!@name event handlers
        //@{

        //! resize
        virtual void resizeEvent( QResizeEvent* );

        //! show
        virtual void showEvent( QShowEvent* );

        //! paint
        virtual void paintEvent( QPaintEvent* );

        //@}

        //!@name margins
        //@{

        //! margins
        const Base::Margins& _margins( void ) const
        { return margins_; }

        //! clear margins
        void _clearMargins( void )
        { margins_.clear(); }

        //! margins
        void _setMargins( const Base::Margins& margins )
        { margins_ = margins; }

        //! margins
        void _setMargins( int margins )
        { margins_ = Base::Margins( margins ); }

        //! outer padding
        Base::Margins _outerPadding( void ) const
        { return outerPadding_; }

        //! clear outer padding
        void _clearOuterPadding( void )
        { outerPadding_.clear(); }

        //! outer padding
        void _setOuterPadding( const Base::Margins& margins )
        { outerPadding_ = margins; }

        //! outer padding
        void _setOuterPadding( int margins )
        { outerPadding_ = Base::Margins( margins ); }

        //@}

        //! background changed
        bool _backgroundChanged( void ) const
        { return backgroundChanged_; }

        //! paint background on devide
        virtual void _paintBackground( QPaintDevice&, const QRect& );

        //! paint main widget on devide
        /*! this must be re-implemented by derived classes */
        virtual void _paint( QPaintDevice&, const QRect& )
        { return; }

        //! update input shape (to prevent input events in outerPadding region)
        virtual void _updateInputShape( void );

        //! update blur region
        virtual void _updateBlurRegion( const QRegion& );

        protected Q_SLOTS:

        //! update background pixmap
        virtual void _updateBackgroundPixmap( void );

        //! update blur region
        virtual void _updateBlurRegion( void )
        {
            if( blurRegion_.isEmpty() ) blurRegion_ = rect().translated( mapTo( window(), QPoint(0,0) ) );
            _updateBlurRegion( blurRegion_ );
        }

        //! toggle inverse colors
        virtual void _toggleInverseColors( bool );

        private Q_SLOTS:

        //! update configuration
        void _updateConfiguration( void );

        private:

        //! actions
        void _installActions( void );

        //!@name actions
        //@{

        //! reload blur region
        QAction* reloadBlurRegionAction_;

        //! inverse colors
        QAction* inverseColorsAction_;

        //@}

        //! true if input shape is set
        bool hasInputShape_;

        //! true when background needs to be reloaded
        bool backgroundChanged_;

        //! foreground
        QColor foregroundColor_;

        //! shadow
        QColor shadowColor_;

        //! tint color
        QColor tintColor_;

        //! foreground intensity
        int foregroundIntensity_;

        //! shadow offset
        int shadowOffset_;

        //! margins
        Base::Margins margins_;

        //! margins
        Base::Margins outerPadding_;

        //! store last blur region
        QRegion blurRegion_;

        #if defined(Q_OS_WIN)
        //! widget pixmap
        /*! it is used as widget storage when using full translucency */
        QPixmap widgetPixmap_;
        #endif

        //! background pixmap
        QPixmap backgroundPixmap_;

    };

};

#endif
