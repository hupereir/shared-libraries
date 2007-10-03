#ifndef FontInfo_h
#define FontInfo_h

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
   \file FontInfo.h
   \brief font formatting
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QCheckBox>

//! font formating
class FontInfo: public QWidget
{

  public:

  //! constructor
  FontInfo( QWidget* parent );

  //! set checkboxes from from
  void setFormat( const unsigned int& format );

  //! get format
  unsigned int format( void ) const;

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
