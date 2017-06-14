#ifndef BaseSvgRenderer_h
#define BaseSvgRenderer_h

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

#include <QByteArray>
#include <QIODevice>
#include <QPaintDevice>
#include <QPalette>
#include <QSvgRenderer>

//* construct pixmap of given size using Svg renderer
namespace Svg
{
    class BaseSvgRenderer: public QSvgRenderer, private Base::Counter<BaseSvgRenderer>
    {

        public:

        //* constructor
        explicit BaseSvgRenderer( void );

        //*@name accessors
        //@{

        //* true if style sheet is used
        bool styleSheetIsUsed( void ) const
        { return styleSheetIsUsed_; }

        //@}

        //*@name modifiers
        //@{

        //* generate style sheet
        void createStyleSheet( QPalette );

        //* load file
        virtual bool load( const QString& );

        //@}

        protected:

        //* uncompress (gzip) input
        QByteArray _tryUncompress( QIODevice& ) const;

        private:

        //* style sheet
        QString styleSheet_;

        //* true if stylesheet was used
        bool styleSheetIsUsed_ = false;

    };

};

#endif
