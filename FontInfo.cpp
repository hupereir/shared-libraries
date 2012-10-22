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

#include "Debug.h"
#include "FontInfo.h"

#include <QtGui/QLayout>

//_____________________________________________
FontInfo::FontInfo( QWidget* parent ):
QWidget( parent )
{
    Debug::Throw( "FontInfo::FontInfo.\n" );
    setLayout( new QHBoxLayout() );
    layout()->setMargin(2);
    layout()->setSpacing(2);
    layout()->addWidget( bold_ = new QCheckBox( "Bold", this ) );
    layout()->addWidget( italic_ = new QCheckBox( "Italic", this ) );
    layout()->addWidget( underline_ = new QCheckBox( "Underline", this ) );
    layout()->addWidget( strike_ = new QCheckBox( "Strike", this ) );
    layout()->addWidget( overline_ = new QCheckBox( "Overline", this ) );
}

//__________________________________________________
void FontInfo::setFormat( FORMAT::TextFormatFlags format )
{
    Debug::Throw( "FontInfo::setFormat.\n" );
    bold_->setChecked( format & FORMAT::Bold );
    italic_->setChecked( format & FORMAT::Italic );
    underline_->setChecked( format & FORMAT::Underline );
    strike_->setChecked( format & FORMAT::Strike );
    overline_->setChecked( format & FORMAT::Overline );
}

//__________________________________________________
unsigned int FontInfo::format( void ) const
{
    Debug::Throw( "FontInfo::format.\n" );
    unsigned int out = FORMAT::Default;
    if( bold_->isChecked() ) out |= FORMAT::Bold;
    if( italic_->isChecked() ) out |= FORMAT::Italic;
    if( underline_->isChecked() ) out |= FORMAT::Underline;
    if( strike_->isChecked() ) out |= FORMAT::Strike;
    if( overline_->isChecked() ) out |= FORMAT::Overline;
    return out;
}
