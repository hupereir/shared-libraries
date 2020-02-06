#ifndef SvgConfiguration_h
#define SvgConfiguration_h

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

#include "base_svg_export.h"
#include "Counter.h"
#include "OptionWidgetList.h"
#include "OptionCheckBox.h"

#include <QWidget>


namespace Svg
{

    //! SVG common configuration
    class BASE_SVG_EXPORT SvgConfiguration: public QWidget, public OptionWidgetList<SvgConfiguration>, private Base::Counter<SvgConfiguration>
    {

        Q_OBJECT

        public:

        //! constructor
        explicit SvgConfiguration( QWidget* parent );

        Q_SIGNALS:

        //! modified
        void modified();

    };

};

#endif
