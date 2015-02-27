
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

#include "TextPosition.h"

#include <QTextBlock>

//______________________________________________________________
TextPosition::TextPosition( QTextDocument* document, int index ):
    Counter( "TextPosition" ),
    paragraph_( 0 ),
    index_( 0 )
{
    if( !document ) return;

    // retrieve block matching position
    QTextBlock block( document->findBlock( index ) );
    if( block.isValid() ) index_ = index - block.position();
    else {

        // if no valid block is found, return position of last character in text
        block = document->end();
        index_ = block.length();

    }

    // rewind to begin of document to get paragraph index
    while( (block = block.previous()).isValid() ) paragraph_++;
}

//______________________________________________________________
int TextPosition::index( QTextDocument* document ) const
{
    if( !document ) return index_;

    // advance until paragraph is matched
    QTextBlock block( document->begin() );
    int paragraph(0);
    while( paragraph < paragraph_ && block.isValid() )
    {
        paragraph++;
        block = block.next();
    }

    // if block is valid advance until index is smaller than current block size
    int index = index_;
    if( block.isValid() )
    {
        while( index > block.length() && block.isValid() )
        {
            block = block.next();
            index -= block.length();
        }
    }

    // if block is valid return found index
    if( block.isValid() ) return block.position() + index;
    else {
        block = document->end();
        return block.position() + block.length();
    }

}
