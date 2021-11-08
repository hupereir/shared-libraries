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

#include "IconSizeComboBox.h"
#include "Debug.h"

#include <QSet>

//_________________________________________________________
IconSizeComboBox::IconSizeComboBox( QWidget* parent, bool custom ):
    QComboBox( parent ),
    Counter( QStringLiteral("IconSizeComboBox") )
{
    Debug::Throw( QStringLiteral("IconSizeComboBox::IconSizeComboBox.\n") );
    setEditable( false );

    // insert items
    const auto& sizes( IconSize::map() );

    // custom sizes
    static const QSet<IconSize::Size> customSizes({ IconSize::Minimum, IconSize::VeryHuge, IconSize::Maximum });
    for( auto&& iter = sizes.begin(); iter != sizes.end(); ++iter )
    {
        // skip some items depending on custom flags
        if( custom && iter.key() == IconSize::Default ) continue;
        if( !custom && customSizes.contains( iter.key() ) ) continue;
        insertItem( QComboBox::count(), iter.value(), iter.key() );
    }

}

//_________________________________________________________
IconSize::Size IconSizeComboBox::iconSize() const
{ return static_cast<IconSize::Size>( itemData( currentIndex() ).value<int>() ); }

//_________________________________________________________
void IconSizeComboBox::selectIconSize( IconSize::Size iconSize )
{

    Debug::Throw() << "IconSizeComboBox::selectIconSize - size: " << iconSize << Qt::endl;
    for( int index = 0; index < QComboBox::count(); ++index )
    {

        const auto local( static_cast<IconSize::Size>( itemData( index ).value<int>() ) );
        if( local == iconSize )
        {
            setCurrentIndex( index );
            break;
        }

    }

}
