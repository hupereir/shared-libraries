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

#ifndef OptionIconBrowsedButton_h
#define OptionIconBrowsedButton_h

/*!
   \file    OptionIconBrowsedButton.h
   \brief   QLineEdit associated to an option for configuration dialogs
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "IconBrowsedButton.h"
#include "OptionWidget.h"
#include "Options.h"

//! QLineEdit associated to an option for configuration dialogs
class OptionIconBrowsedButton: public IconBrowsedButton, public OptionWidget
{

  public:

  //! constructor
  OptionIconBrowsedButton( QWidget* parent, const QString& option_name ):
    IconBrowsedButton( parent ),
    OptionWidget( option_name )
  {}

  //! read value from option
  void read( void )
  { setFile( XmlOptions::get().raw( optionName() ), false ); }

  //! write value to option
  void write( void ) const
  { XmlOptions::get().setRaw( optionName(), file() ); }

};
#endif
