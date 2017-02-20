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

#include "FontInfo.h"
#include "Debug.h"

#include <QLayout>

//_____________________________________________
FontInfo::FontInfo( QWidget* parent ):
QWidget( parent )
{
    Debug::Throw( "FontInfo::FontInfo.\n" );

    // create checkboxes
    setLayout( new QVBoxLayout() );
    layout()->addWidget( checkBoxes_.insert( Format::Bold, new QCheckBox( tr( "Bold" ), this ) ).value() );
    layout()->addWidget( checkBoxes_.insert( Format::Italic, new QCheckBox( tr( "Italic" ), this ) ).value() );
    layout()->addWidget( checkBoxes_.insert( Format::Underline, new QCheckBox( tr( "Underline" ), this ) ).value() );
    layout()->addWidget( checkBoxes_.insert( Format::Strike, new QCheckBox( tr( "Strike" ), this ) ).value() );
    layout()->addWidget( checkBoxes_.insert( Format::Overline, new QCheckBox( tr( "Overline" ), this ) ).value() );

    for( auto&& iter = checkBoxes_.begin(); iter != checkBoxes_.end(); ++iter )
    { connect( iter.value(), SIGNAL(toggled(bool)), SIGNAL(modified())); }
}

//__________________________________________________
void FontInfo::setFormat( Format::TextFormatFlags format )
{
    Debug::Throw( "FontInfo::setFormat.\n" );
    for( CheckBoxMap::iterator iter = checkBoxes_.begin(); iter != checkBoxes_.end(); ++iter )
    { iter.value()->setChecked( format&iter.key() ); }
}

//__________________________________________________
Format::TextFormatFlags FontInfo::format( void ) const
{
    Debug::Throw( "FontInfo::format.\n" );

    Format::TextFormatFlags out = Format::Default;
    for( CheckBoxMap::const_iterator iter = checkBoxes_.constBegin(); iter != checkBoxes_.constEnd(); ++iter )
    { if( iter.value()->isChecked() ) out |= iter.key(); }

    return out;
}
