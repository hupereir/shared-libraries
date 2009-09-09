#ifndef IconSizeMenu_h
#define IconSizeMenu_h

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
  \file IconSizeMenu.h
  \brief provides icon size selection menu
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QMenu>
#include <map>

#include "Counter.h"
#include "CustomToolButton.h"
#include "Debug.h"
#include "IconSize.h"

//! provides icon size selection menu
class IconSizeMenu: public QMenu, public Counter
{

  Q_OBJECT

  public:

  //! constructor
  IconSizeMenu( QWidget* parent = 0 );

  //! select size
  void select( IconSize::Size );

  signals:

  //! emmitted when a new size is selected
  void iconSizeSelected( IconSize::Size );

  private slots:

  //! size selected from action
  void _selected( QAction* );

  private:

  //! action map
  typedef std::map<QAction*, IconSize::Size > ActionMap;

  //! toolbar text action map
  ActionMap actions_;

};

#endif
