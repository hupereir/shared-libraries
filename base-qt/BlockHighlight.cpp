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

#include "BlockHighlight.h"
#include "TextBlockData.h"
#include "TextBlockRange.h"
#include "TextEditor.h"

#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextBlock>

//_______________________________________________________________________
BlockHighlight::BlockHighlight( TextEditor* parent ):
    QObject( parent ),
    Counter( QStringLiteral("BlockHighlight") ),
    parent_( parent )
{ Debug::Throw( QStringLiteral("BlockHighlight::BlockHighlight.\n") ); }

//______________________________________________________________________
void BlockHighlight::clear()
{

    if( cleared_ ) return;

    // loop over all blocks
    for( const auto& block:TextBlockRange( parent_->document() ) )
    {

        if( parent_->textCursor().block() == block && isEnabled() ) continue;

        auto data( static_cast<TextBlockData*>( block.userData() ) );
        if( data && data->hasFlag( TextBlock::CurrentBlock ) )
        {

            // reset flag
            data->setFlag( TextBlock::CurrentBlock, false );

            // mark contents dirty to trigger document update
            _updateEditors();

        }

    }

    cleared_ = true;

}

//______________________________________________________________________
void BlockHighlight::highlight()
{
    if( isEnabled() )
    {
        clear();
        timer_.start(50, this );
    }
}

//______________________________________________________________________
void BlockHighlight::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {
        timer_.stop();
        _highlight();
    } else QObject::timerEvent( event );
}

//______________________________________________________________________
void BlockHighlight::_highlight()
{

    if( !isEnabled() ) return;

    // retrieve current block
    auto block( parent_->textCursor().block() );
    auto data = static_cast<TextBlockData*>( block.userData() );
    if( !data )
    {

        data = new TextBlockData;
        block.setUserData( data );

    } else if( data->hasFlag( TextBlock::CurrentBlock ) ) return;

    // need to redo the clear a second time, forced,
    // in case the previous draw action occured after the previous clear.
    cleared_ = false;
    clear();

    // mark block as current
    data->setFlag( TextBlock::CurrentBlock, true );

    // mark contents dirty to trigger document update
    _updateEditors();

    cleared_ = false;

}

//______________________________________________________________________
void BlockHighlight::_updateEditors()
{

    Base::KeySet<TextEditor> editors( parent_ );
    editors.insert( parent_ );
    for( const auto& editor:editors ) editor->viewport()->update();
}
