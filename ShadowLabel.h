#ifndef ShadowLabel_h
#define ShadowLabel_h
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

#include "Counter.h"

#include <QColor>
#include <QLabel>
#include <QPaintDevice>

namespace Transparency
{
    class ShadowLabel: public QLabel, public Counter
    {

        public:

        //! constructor
        ShadowLabel( QWidget* parent ):
            QLabel( parent ),
            Counter( "Transparency::ShadowLabel" ),
            shadowOffset_( 0 )
        {}

        //! constructor
        ShadowLabel( const QString& text, QWidget* parent ):
            QLabel( text, parent ),
            Counter( "Transparency::ShadowLabel" ),
            shadowOffset_( 0 )
        {}

        //! shadow
        void setShadow( const QColor& color, int offset )
        {
            setShadowColor( color );
            setShadowOffset( offset );
        }

        //! shadow offset
        void setShadowOffset( int value )
        { shadowOffset_ = value; }

        //! shadow color
        void setShadowColor( const QColor& color )
        { shadowColor_ = color; }

        protected:

        virtual void paintEvent( QPaintEvent* );

        private Q_SLOTS:

        //! configuration
        void _updateConfiguration( void )
        {}

        private:

        //! shadow offset
        const int& _shadowOffset( void ) const
        { return shadowOffset_; }

        //! shadow color
        const QColor& _shadowColor( void ) const
        { return shadowColor_; }

        //! shadow offset
        int shadowOffset_;

        //! shadow color
        QColor shadowColor_;


    };

};

#endif
