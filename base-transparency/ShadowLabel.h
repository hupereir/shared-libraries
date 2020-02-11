#ifndef ShadowLabel_h
#define ShadowLabel_h

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

#include <QColor>
#include <QLabel>
#include <QPaintDevice>

namespace Transparency
{
    class BASE_TRANSPARENCY_EXPORT ShadowLabel: public QLabel, private Base::Counter<ShadowLabel>
    {

        Q_OBJECT

        public:

        //* construct from any args that make a QLabel
        template< typename... Args >
            explicit ShadowLabel( Args&&... args ):
            QLabel( std::forward<Args>(args)... ),
            Counter( QStringLiteral("Transparency::ShadowLabel") ),
            shadowOffset_( 0 )
        {}

        //* shadow
        void setShadow( const QColor& color, int offset )
        {
            setShadowColor( color );
            setShadowOffset( offset );
        }

        //* shadow offset
        void setShadowOffset( int value )
        { shadowOffset_ = value; }

        //* shadow color
        void setShadowColor( const QColor& color )
        { shadowColor_ = color; }

        protected:

        void paintEvent( QPaintEvent* ) override;

        private:

        //* shadow offset
        int _shadowOffset() const
        { return shadowOffset_; }

        //* shadow color
        const QColor& _shadowColor() const
        { return shadowColor_; }

        //* shadow offset
        int shadowOffset_;

        //* shadow color
        QColor shadowColor_;


    };

};

#endif
