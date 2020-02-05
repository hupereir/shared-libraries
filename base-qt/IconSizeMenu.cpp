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

#include "IconSizeMenu.h"
#include "CppUtil.h"
#include "Debug.h"

//_____________________________________________________________________________
IconSizeMenu::IconSizeMenu( QWidget* parent, bool custom ):
    QMenu( tr( "Icon size" ), parent ),
    Counter( QStringLiteral("IconSizeMenu") )
{
    Debug::Throw( QStringLiteral("IconSizeMenu::IconSizeMenu.\n") );

    auto group = new QActionGroup( this );
    connect( group, &QActionGroup::triggered, this, &IconSizeMenu::_selected );

    const auto& sizes( IconSize::map() );

    // custom sizes
    static const QList<IconSize::Size> customSizes( { IconSize::Minimum, IconSize::VeryHuge, IconSize::Maximum });

    // generic action
    QAction* action;
    for( auto&& iter = sizes.begin(); iter != sizes.end(); ++iter )
    {

        // skip some items depending on custom flags
        if( custom && iter.key() == IconSize::Default ) continue;
        if( !custom && customSizes.contains( iter.key() ) ) continue;

        addAction( action = new QAction( iter.value(), this ) );
        action->setCheckable( true );
        actions_.insert( action, iter.key() );
        group->addAction( action );
    }

}

//_____________________________________________________________________________
void IconSizeMenu::select( IconSize::Size size )
{

    Debug::Throw( QStringLiteral("IconSizeMenu::select.\n") );
    const auto iter = Base::findByValue( actions_, size );
    if( iter != actions_.end() ) iter.key()->setChecked( true );

}

//_____________________________________________________________________________
void IconSizeMenu::_selected( QAction* action )
{

    Debug::Throw( QStringLiteral("IconSizeMenu::_selected.\n") );

    // find matching actions
    const auto iter = actions_.find( action );
    Q_ASSERT( iter != actions_.end() );
    emit iconSizeSelected( iter.value() );

}
