#ifndef OptionFontInfo_h
#define OptionFontInfo_h

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
   \file OptionFontInfo.h
   \brief font formatting options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <qgrid.h>
#include <qcheckbox.h>

#include "OptionWidget.h"
#include "Options.h"

class CustomListBox;
class QListBoxItem;

//! font formating options
class OptionFontInfo: public QGrid, public OptionWidget
{

  public:

  //! constructor
  OptionFontInfo( QWidget* parent, const std::string& option_name );

  //! read value from option
  void Read( void );

  //! write value to option
  void Write( void ) const;

  private:

  //! bold 
  QCheckBox* bold_;
  
  //! italic
  QCheckBox* italic_;
  
  //! underline
  QCheckBox* underline_;
  
  //! strike
  QCheckBox* strike_;
  
};
#endif
