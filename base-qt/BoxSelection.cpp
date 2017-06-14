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

#include "BoxSelection.h"
#include "TextEditor.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QMimeData>

static const int debugLevel = 1;

//________________________________________________________________________
const QString BoxSelection::mimeType = "text/boxSelection";

//________________________________________________________________________
BoxSelection::BoxSelection( TextEditor* parent ):
    Counter( "BoxSelection" ),
    parent_( parent )
{
    Debug::Throw( debugLevel, "BoxSelection::BoxSelection.\n" );
    updateConfiguration();
}

//________________________________________________________________________
bool BoxSelection::empty( void ) const
{
    return
        !enabled_ ||
        state_ == State::Empty ||
        ( state_ == State::Finished && cursors_.empty() );
}

//________________________________________________________________________
void BoxSelection::synchronize( const BoxSelection& box )
{

    Debug::Throw( debugLevel, "BoxSelection::synchronize.\n" );

    // check enable state.
    if( !checkEnabled() ) return;

    begin_ = box.begin();
    end_ = box.end();
    state_ = box.state();

    QRect old( rect() );
    _updateRect();

    parent_->viewport()->update( parent_->toViewport( old.united( rect() ) ).adjusted( -2, -2, 3, 3 ) );

    return;

}


//________________________________________________________________________
void BoxSelection::updateConfiguration( void )
{

    Debug::Throw( "BoxSelection::updateConfiguration.\n" );

    // retrieve box selection color from options
    QColor color = parent_->palette().color( QPalette::Active, QPalette::Highlight );
    color_ = color;

    // retrieve shading from options
    color.setAlphaF( XmlOptions::get().get<double>("BOX_SELECTION_ALPHA")/100 );
    brush_ = QBrush( color );

    // additional initialization dependening on whether box selection is enabled or not
    enabled_ = false;
    checkEnabled();
}

//________________________________________________________________________
bool BoxSelection::checkEnabled( void )
{

    Debug::Throw( debugLevel, "BoxSelection::checkEnabled.\n" );
    if( isEnabled() ) return true;

    /*
    check if color is valid and font is fixed pitched
    to not modify the previous attributes if disabled
    because they are needed to clear any existing selection
    */
    bool fixed( QFontInfo( parent_->font() ).fixedPitch() );
    enabled_ = fixed && color_.isValid();
    Debug::Throw( debugLevel ) << "BoxSelection::updateConfiguration - isEnabled: " << isEnabled() << endl;
    if( !isEnabled() ) return false;

    // read font attributes
    fontWidth_ = parent_->fontMetrics().width( " " );
    fontHeight_ = parent_->fontMetrics().height();

    return true;
}

//________________________________________________________________________
bool BoxSelection::start( QPoint point )
{
    Debug::Throw( debugLevel, "BoxSelection::start.\n" );
    if( state_ != State::Empty ) return false;

    // store point
    begin_ = end_ = parent_->fromViewport( point );

    // adjust rectangle size
    _updateRect();

    // set parent cursor
    parent_->setTextCursor( parent_->cursorForPosition( cursor_ ) );

    state_ = State::Started;
    return true;
}

//________________________________________________________________________
bool BoxSelection::update( QPoint point )
{
    Debug::Throw( debugLevel, "BoxSelection::update.\n" );
    if( state_ != State::Started ) return false;

    // store end point
    end_ = parent_->fromViewport( point );

    // retrieve old rect
    QRect old( rect() );

    // update with new value
    _updateRect();

    // update parent
    // the adjustment is to account for the pen width
    parent_->viewport()->update( parent_->toViewport( old.united( rect() ).adjusted( -2, -2, 3, 3 ) ) );
    parent_->setTextCursor( parent_->cursorForPosition( cursor_ ) );

    return true;
}

//________________________________________________________________________
bool BoxSelection::finish( QPoint point )
{
    Debug::Throw( debugLevel, "BoxSelection::finish.\n" );
    if( state_ != State::Started ) return false;

    update( point );
    state_ = State::Finished;

    // store and copy to clipboard
    _store();
    toClipboard( QClipboard::Selection );

    return true;
}

//________________________________________________________________________
bool BoxSelection::clear( void )
{
    Debug::Throw( debugLevel, "BoxSelection::clear.\n" );
    if( state_ != State::Finished ) return false;

    // change state and redraw
    state_ = State::Empty;

    // update parent
    parent_->viewport()->update( parent_->toViewport( rect() ).adjusted( -2, -2, 3, 3 ) );

    // clear cursors points and rect
    cursors_.clear();
    cursor_ = begin_ = end_ = QPoint();
    rect_ = QRect();

    return true;
}


//________________________________________________________________________
QString BoxSelection::toString( void ) const
{
    Debug::Throw( debugLevel, "BoxSelection::toString.\n" );

    QString out;

    // retrieve cursor, copy selected text in output string
    CursorList cursors( cursorList() );
    if( cursorList().empty() ) return out;

    // append a new line for all items but the last
    for( int i=0;  i < cursorList().size()-1; i++ )
    { out += cursorList()[i].selectedText().leftJustified( cursors.columnCount() ) + "\n"; }

    // append last item
    out += cursorList().back().selectedText().leftJustified( cursors.columnCount() );

    return out;

}

//________________________________________________________________________
bool BoxSelection::fromString( QString input )
{

    Debug::Throw( debugLevel, "BoxSelection::fromString.\n" );

    // check state
    if( state() != State::Finished ) return false;

    // replace all tab characters by emulated tabs
    input.replace( "\t", parent_->emulatedTabCharacter() );

    // try split
    QStringList inputList( input.split( "\n" ) );

    // retrieve maximum length
    int columns(0);
    for( int i=0; i < inputList.size(); i++ )
    { columns = qMax( columns, inputList[i].length() ); }

    // if there are more lines in current box than in the selection, fill with blank lines
    for( int i = inputList.size(); i < cursors_.size(); i++ )
    { inputList << QString( columns, ' ' ); }

    // loop over items and cursors, replace strings
    QTextCursor cursor( parent_->textCursor() );
    cursor.beginEditBlock();
    for( int i=0; i < qMin( inputList.size(), cursors_.size() ); i++ )
    {

        cursor.setPosition( cursors_[i].anchor() );

        // if cursor has no selection, one need to make sur that there is enough white spaces in the line before copying the new string
        // this hack is necessary because for some reason cursor.movePosition( QTextCursor::StartOfLine ) fails.
        int extra_length = cursors_.firstColumn() ;
        int old_bottom( parent_->cursorRect( cursor ).bottom() );
        int new_bottom( 0 );
        while( cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor ) && (new_bottom =  parent_->cursorRect( cursor ).bottom() ) == old_bottom )
        {
            extra_length--;
            old_bottom = new_bottom;
        }

        // move to current cursor end of the selection
        cursor.setPosition( cursors_[i].position(), QTextCursor::KeepAnchor );

        // insert new text
        cursor.insertText( inputList[i].rightJustified( extra_length + inputList[i].size() ).leftJustified( extra_length + columns ) );

    }

    // if there are more lines in inputList than in current selection, insert new lines
    for( int i = cursors_.size(); i < inputList.size(); i++ )
    {
        cursor.insertText( QString( '\n' ) +
            inputList[i]
            .rightJustified( cursors_.firstColumn() + inputList[i].size() )
            .leftJustified( cursors_.firstColumn() + columns ) );
    }

    cursor.endEditBlock();
    parent_->setTextCursor( cursor );

    return true;

}

//________________________________________________________________________
bool BoxSelection::toClipboard( const QClipboard::Mode& mode ) const
{
    Debug::Throw(debugLevel) << "BoxSelection::toClipboard - mode: " << ( mode == QClipboard::Selection ? "Selection":"Clipboard" ) << endl;

    // check if selection mode is available
    if( mode == QClipboard::Selection && !qApp->clipboard()->supportsSelection() ) return false;

    // check if state is ok
    if( state() != State::Finished ) return false;

    // check if stored text makes sense
    QString selection( toString() );
    if( selection.isEmpty() ) return false;

    // store text into MimeData
    QMimeData *data( new QMimeData );
    data->setText( selection );
    data->setData( mimeType, qPrintable( selection ) );

    // copy selected text to clipboard
    qApp->clipboard()->setMimeData( data, mode );

    return true;

}

//________________________________________________________________________
bool BoxSelection::fromClipboard( const QClipboard::Mode& mode )
{

    Debug::Throw( debugLevel, "BoxSelection::fromClipboard.\n" );

    // check mode
    if( mode == QClipboard::Selection && !qApp->clipboard()->supportsSelection() ) return false;

    // check mime data availability
    const QMimeData* data( qApp->clipboard()->mimeData() );
    if( !data )
    {
        Debug::Throw(debugLevel) << "BoxSelection::fromClipboard - no mimeData" << endl;
        return false;
    }

    // try retrieve directly from formated input
    return data->hasText() ? fromString( data->text() ) : false;

}

//________________________________________________________________________
bool BoxSelection::removeSelectedText( void ) const
{
    Debug::Throw( debugLevel, "BoxSelection::removeSelectedText.\n" );

    // check if state is ok
    if( state() != State::Finished || cursorList().empty() ) return false;

    QTextCursor stored( parent_->textCursor() );
    QTextCursor cursor( parent_->textCursor() );
    cursor.beginEditBlock();
    for( const auto& savedCursor:cursorList() )
    {

        // select line and remove
        cursor.setPosition( savedCursor.anchor() );
        cursor.setPosition( savedCursor.position(), QTextCursor::KeepAnchor );
        cursor.removeSelectedText();

    }

    cursor.endEditBlock();

    // restore cursor
    if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
    {
        stored.setPosition( cursorList().front().anchor() );
    } else stored.setPosition( cursor.position() );

    parent_->setTextCursor( stored );

    return true;

}

//________________________________________________________________________
bool BoxSelection::toUpper( void )
{
    Debug::Throw( debugLevel, "BoxSelection::toUpper.\n" );

    // check if state is ok
    if( state() != State::Finished || cursorList().empty() ) return false;

    QTextCursor stored( parent_->textCursor() );
    QTextCursor cursor( parent_->textCursor() );
    cursor.beginEditBlock();
    for( const auto& savedCursor:cursorList() )
    {

        // select line and remove
        cursor.setPosition( savedCursor.anchor() );
        cursor.setPosition( savedCursor.position(), QTextCursor::KeepAnchor );
        cursor.insertText( cursor.selectedText().toUpper() );

    }

    cursor.endEditBlock();

    // restore cursor
    if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
    {
        stored.setPosition( cursorList().front().anchor() );
    } else stored.setPosition( cursor.position() );

    parent_->setTextCursor( stored );

    _store();
    toClipboard( QClipboard::Selection );

    return true;

}

//________________________________________________________________________
bool BoxSelection::toLower( void )
{
    Debug::Throw( debugLevel, "BoxSelection::toLower.\n" );

    // check if state is ok
    if( state() != State::Finished || cursorList().empty() ) return false;

    QTextCursor stored( parent_->textCursor() );
    QTextCursor cursor( parent_->textCursor() );
    cursor.beginEditBlock();
    for( const auto& savedCursor:cursorList() )
    {

        // select line and remove
        cursor.setPosition( savedCursor.anchor() );
        cursor.setPosition( savedCursor.position(), QTextCursor::KeepAnchor );
        cursor.insertText( cursor.selectedText().toLower() );

    }

    cursor.endEditBlock();

    // restore cursor
    if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
    {
        stored.setPosition( cursorList().front().anchor() );
    } else stored.setPosition( cursor.position() );

    parent_->setTextCursor( stored );

    _store();
    toClipboard( QClipboard::Selection );
    return true;

}

//________________________________________________________________________
bool BoxSelection::mergeCharFormat( const QTextCharFormat& format ) const
{

    Debug::Throw( debugLevel, "BoxSelection::mergeCharFormat.\n" );

    // trailing space regexp
    static QRegExp regexp( "\\s+$" );

    // check if state is ok
    if( state() != State::Finished || cursorList().empty() ) return false;

    QTextCursor stored( parent_->textCursor() );
    QTextCursor cursor( parent_->textCursor() );
    cursor.beginEditBlock();
    for( const auto& savedCursor:cursorList() )
    {

        // select line and remove
        cursor.setPosition( savedCursor.anchor() );
        cursor.setPosition( savedCursor.position(), QTextCursor::KeepAnchor );

        // get selection, look for trailing spaces
        QString text( cursor.selectedText() );
        if( regexp.indexIn( text ) >= 0 )
        { cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, regexp.matchedLength() ); }

        cursor.mergeCharFormat( format );

    }

    cursor.endEditBlock();

    // restore cursor
    if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
    {
        stored.setPosition( cursorList().front().anchor() );
    } else stored.setPosition( cursor.position() );

    parent_->setTextCursor( stored );

    return true;

}

//________________________________________________________________________
void BoxSelection::_updateRect( void )
{
    Debug::Throw( debugLevel, "BoxSelection::_updateRect.\n" );
    int xMin( qMin( begin_.x(), end_.x() ) );
    int xMax( qMax( begin_.x(), end_.x() ) );

    int yMin( qMin( begin_.y(), end_.y() ) );
    int yMax( qMax( begin_.y(), end_.y() ) );

    QPoint begin( xMin - (xMin%fontWidth_) + 2, yMin - (yMin%fontHeight_) + 2 );
    QPoint end( xMax - (xMax%fontWidth_) + 1, yMax + fontHeight_ - (yMax%fontHeight_) );

    // decide location of cursor point
    cursor_.setX( begin_.x() < end_.x() ? end.x() : begin.x() );
    cursor_.setY( begin_.y() < end_.y() ?  end.y() : begin.y() + fontHeight_ - 1 );
    cursor_ = parent_->toViewport( cursor_ );

    // compute rect
    rect_ = QRect( begin, end );

    return;

}

//________________________________________________________________________
void BoxSelection::_store( void )
{
    Debug::Throw( debugLevel, "BoxSelection::_store.\n" );

    // retrieve box selection size
    int firstColumn = rect().left() / fontWidth_;
    int columns = rect().width() / fontWidth_;
    int rows = rect().height() / fontHeight_ + 1;

    Debug::Throw() << "BoxSelection::_store - [" << firstColumn << "," << columns << "," << rows << "]" << endl;

    // translate rect
    QRect local( parent_->toViewport( rect() ) );
    cursors_ = CursorList( firstColumn, columns );
    for( int row = 0; row < rows; row ++ )
    {

        // vertical offset for this row
        QPoint voffset( 0, (int)(fontHeight_*( 0.5+row )) );
        QPoint begin( local.topLeft() + voffset );
        QPoint end( local.topRight() + voffset );

        Debug::Throw() << "BoxSelection::_store -"
            << " begin: (" << begin.x() << "," << begin.y() << ")"
            << " end: (" << end.x() << "," << end.y() << ")"
            << endl;

        QTextCursor cursor( parent_->cursorForPosition( begin ) );
        Debug::Throw()
            << "BoxSelection::_store -"
            << " begin: " << parent_->cursorForPosition( begin ).position()
            << " end: " << parent_->cursorForPosition( end ).position()
            << endl;

        // check if cursor is at end of block, and move at end
        if( !cursor.atBlockEnd() )
        { cursor.setPosition( parent_->cursorForPosition( end ).position(), QTextCursor::KeepAnchor ); }

        // store in list
        cursors_ << cursor;

    }

    return;

}
