#ifndef ServerConfiguration_h
#define ServerConfiguration_h

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

#include "base_server_export.h"
#include <QWidget>
#include "Counter.h"
#include "OptionWidgetList.h"

namespace Server
{

  //! transparency common configuration
  class BASE_SERVER_EXPORT ServerConfiguration: public QWidget, public OptionWidgetList<ServerConfiguration>, private Base::Counter<ServerConfiguration>
  {

    Q_OBJECT

    public:

    //! constructor
    explicit ServerConfiguration( QWidget*, const QString &title = QString() );

    Q_SIGNALS:

    //! modified
    void modified();

  };
};

#endif
