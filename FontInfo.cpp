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
   \file FontInfo.cpp
   \brief font formatting options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QHBoxLayout>

#include "Debug.h"
#include "FontInfo.h"
#include "TextFormat.h"



//_____________________________________________
FontInfo::FontInfo( QWidget* parent ):
  QWidget( parent )
{
  Debug::Throw( "FontInfo::FontInfo.\n" );
  setLayout( new QHBoxLayout() );
  layout()->setMargin(2);
  layout()->setSpacing(2);
  layout()->addWidget( bold_ = new QCheckBox( "&Bold", this ) );
  layout()->addWidget( italic_ = new QCheckBox( "&Italic", this ) );
  layout()->addWidget( underline_ = new QCheckBox( "&Underline", this ) );
  layout()->addWidget( strike_ = new QCheckBox( "&Strike", this ) );
}

//__________________________________________________
void FontInfo::setFormat( const unsigned int& format )
{
  Debug::Throw( "FontInfo::setFormat.\n" );
  bold_->setChecked( format & FORMAT::BOLD );
  italic_->setChecked( format & FORMAT::ITALIC );
  underline_->setChecked( format & FORMAT::UNDERLINE );
  strike_->setChecked( format & FORMAT::STRIKE );
}

//__________________________________________________
unsigned int FontInfo::format( void ) const
{
  Debug::Throw( "FontInfo::format.\n" );
  unsigned int out = FORMAT::DEFAULT;
  if( bold_->isChecked() ) out |= FORMAT::BOLD;
  if( italic_->isChecked() ) out |= FORMAT::ITALIC;
  if( underline_->isChecked() ) out |= FORMAT::UNDERLINE;
  if( strike_->isChecked() ) out |= FORMAT::STRIKE;
  return out;
}
