#ifndef _AutoSpellConfiguration_h_
#define _AutoSpellConfiguration_h_

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
#include "OptionWidgetList.h"

#include <QGroupBox>

namespace SPELLCHECK
{

    //! Spell common configuration
    class AutoSpellConfiguration: public QGroupBox, public OptionWidgetList, public Counter
    {

        Q_OBJECT

        public:

        //! constructor
        AutoSpellConfiguration( QWidget* parent );

    };

}
#endif
