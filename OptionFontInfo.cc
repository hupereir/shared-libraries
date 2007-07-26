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
   \file OptionFontInfo.cc
   \brief font formatting options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QHBoxLayout>

#include "Debug.h"
#include "OptionFontInfo.h"
#include "XmlOptions.h"
#include "TextFormat.h"

using namespace std;

//_____________________________________________
OptionFontInfo::OptionFontInfo( QWidget* parent, const string& option_name ):
  QWidget( parent ),
  OptionWidget( option_name )
{
  Debug::Throw( "OptionFontInfo::OptionFontInfo.\n" );
  setLayout( new QHBoxLayout() );
  layout()->setMargin(2);
  layout()->setSpacing(2);
  layout()->addWidget( bold_ = new QCheckBox( "bold", this ) );
  layout()->addWidget( italic_ = new QCheckBox( "italic", this ) );
  layout()->addWidget( underline_ = new QCheckBox( "underline", this ) );
  layout()->addWidget( strike_ = new QCheckBox( "strike", this ) );
}

//__________________________________________________
void OptionFontInfo::read( void )
{
  unsigned int format = XmlOptions::get().get<unsigned int>( optionName() );
  bold_->setChecked( format & FORMAT::BOLD );
  italic_->setChecked( format & FORMAT::ITALIC );
  underline_->setChecked( format & FORMAT::UNDERLINE );
  strike_->setChecked( format & FORMAT::STRIKE );
}

//__________________________________________________
void OptionFontInfo::write( void ) const
{
  unsigned int format = FORMAT::DEFAULT;
  if( bold_->isChecked() ) format |= FORMAT::BOLD;
  if( italic_->isChecked() ) format |= FORMAT::ITALIC;
  if( underline_->isChecked() ) format |= FORMAT::UNDERLINE;
  if( strike_->isChecked() ) format |= FORMAT::STRIKE;
  XmlOptions::get().set<unsigned int>( optionName(), format );
}
