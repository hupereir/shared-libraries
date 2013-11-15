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

#include "TextEditor.h"
#include "Debug.h"
#include "LineNumberDisplay.h"
#include "LineNumberDisplay.moc"
#include "XmlOptions.h"

#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>

//____________________________________________________________________________
LineNumberDisplay::LineNumberDisplay(TextEditor* editor):
    QObject( editor ),
    Counter( "LineNumberDisplay" ),
    editor_( editor ),
    needsUpdate_( true ),
    width_( 0 )
{

    Debug::Throw( "LineNumberDisplay::LineNumberDisplay.\n" );

    connect( &editor_->wrapModeAction(), SIGNAL(toggled(bool)), SLOT(needUpdate()) );

    // document connections
    connect( editor_->document(), SIGNAL(blockCountChanged(int)), SLOT(_blockCountChanged()) );
    connect( editor_->document(), SIGNAL(contentsChanged()), SLOT(_contentsChanged()) );

}

//__________________________________________
LineNumberDisplay::~LineNumberDisplay()
{ Debug::Throw( "LineNumberDisplay::~LineNumberDisplay.\n" );}

//__________________________________________
void LineNumberDisplay::synchronize( LineNumberDisplay* display )
{

    Debug::Throw( "LineNumberDisplay::synchronize.\n" );

    // copy members
    lineNumberData_ = display->lineNumberData_;
    needsUpdate_ = display->needsUpdate_;
    width_ = display->width_;

    // re-initialize connections
    connect( editor_->document(), SIGNAL(blockCountChanged(int)), SLOT(_blockCountChanged()) );
    connect( editor_->document(), SIGNAL(contentsChanged()), SLOT(_contentsChanged()) );

}

//__________________________________________
bool LineNumberDisplay::updateWidth( const int& count )
{
    Debug::Throw( "LineNumberDisplay::updateWidth.\n" );

    int new_width( editor_->fontMetrics().width( QString::number( count ) ) + 14 );
    if( width() == new_width ) return false;
    width_ = new_width;

    Debug::Throw() << "LineNumberDisplay::updateWidth - count: " << count << " new_width: " << new_width << endl;
    return true;
}

//__________________________________________
void LineNumberDisplay::clear( void )
{
    Debug::Throw( "LineNumberDisplay::clear.\n" );
    lineNumberData_.clear();
    needsUpdate_ = true;
}

//__________________________________________
void LineNumberDisplay::paint( QPainter& painter )
{
    // update line number data if needed
    if( needsUpdate_ ) _updateLineNumberData();
    needsUpdate_ = false;

    // font metric and offset
    const QFontMetrics metric( editor_->fontMetrics() );

    // calculate dimensions
    int yOffset = editor_->verticalScrollBar()->value() - editor_->frameWidth();
    int height( editor_->contentsRect().height() );

    // translate
    height += yOffset;

    // get begin and end cursor positions
    int firstIndex = editor_->cursorForPosition( QPoint( 0, 0 ) ).position();
    int lastIndex = editor_->cursorForPosition( QPoint( 0, editor_->height() ) ).position();

    // loop over data
    QTextBlock block( editor_->document()->begin() );
    unsigned int id( 0 );

    for( LineNumberData::List::iterator iter = lineNumberData_.begin(); iter != lineNumberData_.end(); ++iter )
    {

        // skip if block is not (yet) in window
        if( iter->cursor() < firstIndex ) continue;

        // stop if block is outside (below) window
        if( iter->cursor() > lastIndex ) break;

        // check validity
        if( !iter->isValid() ) _updateLineNumberData( block, id, *iter );

        // check position
        if( iter->isValid() && iter->position() > height ) continue;

        QString numtext( QString::number( iter->lineNumber() ) );
        painter.drawText(
            0, iter->position(), width_-8,
            metric.lineSpacing(),
            Qt::AlignRight | Qt::AlignTop,
            numtext );

    }

}

//________________________________________________________
void LineNumberDisplay::_contentsChanged( void )
{

    // if text is wrapped, line number data needs update at next update
    /* note: this could be further optimized if one retrieve the position at which the contents changed occured */
    if( editor_->lineWrapMode() != TextEditor::NoWrap )
    { needUpdate(); }

}

//________________________________________________________
void LineNumberDisplay::_blockCountChanged( void )
{

    // nothing to be done if wrap mode is not NoWrap, because
    // it is handled in the _contentsChanged slot.
    if( editor_->lineWrapMode() == TextEditor::NoWrap )
    { needUpdate(); }

}

//________________________________________________________
void LineNumberDisplay::_updateLineNumberData( void )
{

    lineNumberData_.clear();

    // get document
    unsigned int id( 0 );
    unsigned int block_count( 1 );
    QTextDocument &document( *editor_->document() );
    for( QTextBlock block = document.begin(); block.isValid(); block = block.next(), id++ )
    {

        // insert new data
        lineNumberData_ << LineNumberData( id, block_count, block.position() );

        // update block count
        block_count += editor_->blockCount( block );

    }

    return;

}

//________________________________________________________
void LineNumberDisplay::_updateLineNumberData( QTextBlock& block, unsigned int& id, LineNumberDisplay::LineNumberData& data ) const
{
    Q_ASSERT( !data.isValid() );

    // find block matching data id
    if( data.id() < id ) { for( ; data.id() < id && block.isValid(); block = block.previous(), id-- ) {} }
    else if( data.id() > id ) { for( ; data.id() > id && block.isValid(); block = block.next(), id++ ) {} }
    Q_ASSERT( block.isValid() );

    data.setPosition( (int)block.layout()->position().y() );

}
