#ifndef TransparencyConfiguration_h
#define TransparencyConfiguration_h

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
#include "OptionWidgetList.h"

#include <QWidget>

namespace Transparency
{
    //! transparency common configuration
    class TransparencyConfiguration: public QWidget, public OptionWidgetList, public Counter
    {

        Q_OBJECT

        public:

        //! flags
        enum Flag
        {
            Foreground = 1<<0,
            Background = 1<<1,
            All = Foreground|Background
        };

        Q_DECLARE_FLAGS( Flags, Flag );

        //! constructor
        TransparencyConfiguration( QWidget* parent, Flags = Foreground );

        Q_SIGNALS:

        //! modified
        void modified( void );

        protected:

        //! foreground configuration
        void _foregroundConfiguration( QWidget* );

        //! background configuration
        void _backgroundConfiguration( QWidget* );


    };

}

#endif
