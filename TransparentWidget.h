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

/*!
\file    TransparentWidget.h
\brief   transparent widget
\author  Hugo Pereira
\version $Revision$
\date    $Date$
*/

#include <QAction>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>

#include "Counter.h"

//! clock widgets
namespace TRANSPARENCY {

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

        //! update background
        QAction& updateBackgroundAction( void ) const
        { return *updateBackgroundAction_; }

        //! reload background action
        QAction& reloadBackgroundAction( void ) const
        { return *reloadBackgroundAction_; }

        //@}

        //! window opacity
        /*! this overloads the QWidget method to hack for WIN system */
        void setWindowOpacity( double );

        //! background changed
        void setBackgroundChanged( const bool& value )
        { backgroundChanged_ = value; }

        public slots:

        //! force reloading of the background
        virtual void setBackgroundChanged( void );

        protected:

        //! enable/disable transparency
        virtual void _setTransparent( const bool& value )
        {
            if( value == transparent_ ) return;
            transparent_ = value;
            setBackgroundChanged( true );
        }

        //! enable/disable transparency
        virtual void _setBlurEnabled( const bool& value )
        {
            if( value == blurEnabled_ ) return;
            blurEnabled_ = value;
            setBackgroundChanged( true );
        }

        //! transparency
        virtual const bool& _transparent( void ) const
        { return transparent_; }

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

        //! opacity
        const double& _opacity( void ) const
        { return opacity_; }

        //!@name event handlers
        //@{

        //! move
        virtual void moveEvent( QMoveEvent* );

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

        //! background changed
        const bool& _backgroundChanged( void ) const
        { return backgroundChanged_; }

        //! paint background on devide
        virtual void _paintBackground( QPaintDevice&, const QRect& );

        //! paint main widget on devide
        /*! this must be re-implemented by derived classes */
        virtual void _paint( QPaintDevice&, const QRect& ) = 0;

        protected slots:

        //! reload background
        virtual void _reloadBackground( void );

        //! update background pixmap
        virtual void _updateBackgroundPixmap( void );

        private slots:

        //! update configuration
        void _updateConfiguration( void );

        private:

        //! actions
        void _installActions( void );

        //!@name actions
        //@{

        //! update background
        QAction *updateBackgroundAction_;

        //! reload background
        QAction* reloadBackgroundAction_;

        //@}

        //! transparency enabled
        bool transparent_;

        //! true when background needs to be reloaded
        bool backgroundChanged_;

        //! tint color
        QColor tintColor_;

        //! true when pointer is in window
        bool highlighted_;

        //! true when blur is enabled
        bool blurEnabled_;

        //! window opacity
        double opacity_;

        //! tint color
        QColor highlightColor_;

        #ifdef Q_WS_WIN
        //! widget pixmap
        /*! it is used as widget storage when using full translucency */
        QPixmap widgetPixmap_;
        #endif

        //! background pixmap
        QPixmap backgroundPixmap_;

    };

};

#endif
