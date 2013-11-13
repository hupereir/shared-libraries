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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Debug.h"
#include "FontInfo.h"
#include "FontInfo.moc"

#include <QLayout>

//_____________________________________________
FontInfo::FontInfo( QWidget* parent ):
QWidget( parent )
{
    Debug::Throw( "FontInfo::FontInfo.\n" );
    setLayout( new QHBoxLayout() );
    layout()->setMargin(2);
    layout()->setSpacing(2);
    layout()->addWidget( bold_ = new QCheckBox( tr( "Bold" ), this ) );
    layout()->addWidget( italic_ = new QCheckBox( tr( "Italic" ), this ) );
    layout()->addWidget( underline_ = new QCheckBox( tr( "Underline" ), this ) );
    layout()->addWidget( strike_ = new QCheckBox( tr( "Strike" ), this ) );
    layout()->addWidget( overline_ = new QCheckBox( tr( "Overline" ), this ) );
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
