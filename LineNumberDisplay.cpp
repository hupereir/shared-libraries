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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software , Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "TextEditor.h"
#include "Debug.h"
#include "LineNumberDisplay.h"
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

    connect( &_editor().wrapModeAction(), SIGNAL( toggled( bool ) ), SLOT( needUpdate() ) );

    // document connections
    connect( _editor().document(), SIGNAL( blockCountChanged( int ) ), SLOT( _blockCountChanged() ) );
    connect( _editor().document(), SIGNAL( contentsChanged() ), SLOT( _contentsChanged() ) );

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
    connect( _editor().document(), SIGNAL( blockCountChanged( int ) ), SLOT( _blockCountChanged() ) );
    connect( _editor().document(), SIGNAL( contentsChanged() ), SLOT( _contentsChanged() ) );

}

//__________________________________________
bool LineNumberDisplay::updateWidth( const int& count )
{
    Debug::Throw( "LineNumberDisplay::updateWidth.\n" );

    int new_width( _editor().fontMetrics().width( QString::number( count ) ) + 14 );
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
    if( needsUpdate_ )
    { _updateLineNumberData(); }
    needsUpdate_ = false;

    // font metric and offset
    const QFontMetrics metric( _editor().fontMetrics() );

    // calculate dimensions
    int y_offset = _editor().verticalScrollBar()->value() - _editor().frameWidth();
    int height( _editor().contentsRect().height() );

    // translate
    height += y_offset;

    // get begin and end cursor positions
    int first_index = _editor().cursorForPosition( QPoint( 0, 0 ) ).position();
    int last_index = _editor().cursorForPosition( QPoint( 0, _editor().height() ) ).position();

    // loop over data
    QTextBlock block( _editor().document()->begin() );
    unsigned int id( 0 );

    for( LineNumberData::List::iterator iter = lineNumberData_.begin(); iter != lineNumberData_.end(); ++iter )
    {

        // skip if block is not (yet) in window
        if( iter->cursor() < first_index ) continue;

        // stop if block is outside (below) window
        if( iter->cursor() > last_index ) break;

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
    if( _editor().lineWrapMode() != QTextEdit::NoWrap )
    { needUpdate(); }

}

//________________________________________________________
void LineNumberDisplay::_blockCountChanged( void )
{

    // nothing to be done if wrap mode is not NoWrap, because
    // it is handled in the _contentsChanged slot.
    if( _editor().lineWrapMode() == QTextEdit::NoWrap )
    { needUpdate(); }

}

//________________________________________________________
void LineNumberDisplay::_updateLineNumberData( void )
{

    lineNumberData_.clear();

    // get document
    unsigned int id( 0 );
    unsigned int block_count( 1 );
    QTextDocument &document( *_editor().document() );
    for( QTextBlock block = document.begin(); block.isValid(); block = block.next(), id++ )
    {

        // insert new data
        lineNumberData_ << LineNumberData( id, block_count, block.position() );

        // update block count
        block_count += _editor().blockCount( block );

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

    QRectF rect( _editor().document()->documentLayout()->blockBoundingRect( block ) );
    data.setPosition( (int)block.layout()->position().y() );

}
