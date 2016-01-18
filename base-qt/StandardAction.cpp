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

#include "StandardAction.h"

#include "BaseIconNames.h"
#include "IconEngine.h"

//_________________________________________________________
StandardAction::StandardAction( StandardAction::ActionType type, QObject* parent ):
    QAction( parent ),
    Counter( "StandardAction" )
{

    switch( type )
    {

        case Undo:
        setIcon( IconEngine::get( IconNames::Undo ) );
        setText( tr( "Undo" ) );
        setShortcut( QKeySequence::Undo );
        break;

        case Redo:
        setIcon( IconEngine::get( IconNames::Redo ) );
        setText( tr( "Redo" ) );
        setShortcut( QKeySequence::Redo );
        break;

        case Cut:
        setIcon( IconEngine::get( IconNames::Cut ) );
        setText( tr( "Cut" ) );
        setShortcut( QKeySequence::Cut );
        break;

        case Copy:
        setIcon( IconEngine::get( IconNames::Copy ) );
        setText( tr( "Copy" ) );
        setShortcut( QKeySequence::Copy );
        break;

        case Paste:
        setIcon( IconEngine::get( IconNames::Paste ) );
        setText( tr( "Paste" ) );
        setShortcut( QKeySequence::Paste );
        break;

        default: break;
    }



}
