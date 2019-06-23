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

#include "CppUtil.h"
#include "Debug.h"

#include <QLayout>

//_____________________________________________
FontInfo::FontInfo( QWidget* parent ):
QWidget( parent )
{
    Debug::Throw( "FontInfo::FontInfo.\n" );

    // create checkboxes
    setLayout( new QVBoxLayout );
    checkBoxes_ = Base::makeT<CheckBoxMap>( {
        {  TextFormat::Bold, new QCheckBox( tr( "Bold" ), this ) },
        {  TextFormat::Italic, new QCheckBox( tr( "Italic" ), this ) },
        {  TextFormat::Underline, new QCheckBox( tr( "Underline" ), this ) },
        {  TextFormat::Strike, new QCheckBox( tr( "Strike" ), this ) },
        {  TextFormat::Overline, new QCheckBox( tr( "Overline" ), this ) },
    });

    for( auto&& iterator = checkBoxes_.begin(); iterator != checkBoxes_.end(); ++iterator )
    {
        layout()->addWidget( iterator.value() );
        connect( iterator.value(), SIGNAL(toggled(bool)), SIGNAL(modified()));
    }

}

//__________________________________________________
void FontInfo::setFormat( TextFormat::Flags format )
{
    Debug::Throw( "FontInfo::setFormat.\n" );
    for( CheckBoxMap::iterator iter = checkBoxes_.begin(); iter != checkBoxes_.end(); ++iter )
    { iter.value()->setChecked( format&iter.key() ); }
}

//__________________________________________________
TextFormat::Flags FontInfo::format() const
{
    Debug::Throw( "FontInfo::format.\n" );

    TextFormat::Flags out = TextFormat::Default;
    for( CheckBoxMap::const_iterator iter = checkBoxes_.constBegin(); iter != checkBoxes_.constEnd(); ++iter )
    { if( iter.value()->isChecked() ) out |= iter.key(); }

    return out;
}
