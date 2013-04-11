#ifndef _TransparentWidget_h_
#define _TransparentWidget_h_
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

#include "Counter.h"
#include "Margins.h"

#include <QAction>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

namespace TRANSPARENCY
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

        //! background changed
        void setBackgroundChanged( const bool& value )
        { backgroundChanged_ = value; }

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

        public slots:

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

        //! highlight
        virtual void _setHighlightColor( const QColor& );

        //! highlight color
        virtual const QColor& _highlightColor( void ) const
        { return highlightColor_; }

        //! shadow offset
        virtual void _setShadowOffset( int value )
        { shadowOffset_ = value; }

        //! foreground intensity
        virtual void _setForegroundIntensity( int value );

        //! highlighted
        virtual void _setHighlighted( bool value )
        { highlighted_ = value; }

        //! highlighted
        virtual const bool& _highlighted( void ) const
        { return highlighted_; }

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

        //! enter event
        virtual void enterEvent( QEvent *event );

        //! leave event
        virtual void leaveEvent( QEvent *event );

        //! paint
        virtual void paintEvent( QPaintEvent* );

        //@}

        //!@name margins
        //@{

        //! margins
        const BASE::Margins& _margins( void ) const
        { return margins_; }

        //! clear margins
        void _clearMargins( void )
        { margins_.clear(); }

        //! margins
        void _setMargins( const BASE::Margins& margins )
        { margins_ = margins; }

        //! margins
        void _setMargins( int margins )
        { margins_ = BASE::Margins( margins ); }

        //! outer padding
        BASE::Margins _outerPadding( void ) const
        { return outerPadding_; }

        //! clear outer padding
        void _clearOuterPadding( void )
        { outerPadding_.clear(); }

        //! outer padding
        void _setOuterPadding( const BASE::Margins& margins )
        { outerPadding_ = margins; }

        //! outer padding
        void _setOuterPadding( int margins )
        { outerPadding_ = BASE::Margins( margins ); }

        //@}

        //! background changed
        const bool& _backgroundChanged( void ) const
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

        protected slots:

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

        private slots:

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

        //! true when pointer is in window
        bool highlighted_;

        //! margins
        BASE::Margins margins_;

        //! margins
        BASE::Margins outerPadding_;

        //! store last blur region
        QRegion blurRegion_;

        //! tint color
        QColor highlightColor_;

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
