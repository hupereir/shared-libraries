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

#include "LogWidget.h"
#include "TextFormat.h"
#include "QtUtil.h"
#include "XmlOptions.h"

//______________________________________________________
LogWidget::LogWidget( QWidget* parent ):
    TextEditor( parent )
{
    Debug::Throw( "LogWidget::LogWidget.\n" );
    setReadOnly( true );
    setWrapFromOptions( false );

    connect( verticalScrollBar(), SIGNAL(sliderMoved(int)), SLOT(_verticalScrollBarMoved(int)) );

}

//______________________________________________________
void LogWidget::setOptionName( const QString& value )
{
    Debug::Throw( "LogWidget::setOptionName.\n" );
    if( optionName_ == value ) return;

    optionName_ = value;
    if( _hasOptionName() )
    {
        if( XmlOptions::get().contains( _wrapOptionName() ) )
        {

            wrapModeAction().setChecked( XmlOptions::get().get<bool>( _wrapOptionName() ) );

        } else {

            XmlOptions::get().set<bool>( _wrapOptionName(), lineWrapMode() == TextEditor::WidgetWidth );

        }
    }
}

//_____________________________________________________________
void LogWidget::append( const QString& text, Format::TextFormatFlags format, const QColor& color, int verbosity )
{

    // check verbosity
    if( verbosity > verbosity_ ) return;

    // move cursor to end of document
    QTextCursor cursor( textCursor() );
    cursor.movePosition( QTextCursor::End );
    QTextCharFormat charFormat;

    // color
    if( color.isValid() ) charFormat.setForeground( color );
    else charFormat.setForeground( palette().color( QPalette::Text ) );

    if( format )
    {
        if( format&Format::LargeFont )
        { charFormat.setFont( QtUtil::titleFont( charFormat.font() ) ); }

        charFormat.setFontWeight( (format&Format::Bold) ? QFont::Bold : QFont::Normal );
        charFormat.setFontItalic( format&Format::Italic );
        charFormat.setFontUnderline( format&Format::Underline );
        charFormat.setFontStrikeOut( format&Format::Strike );
        charFormat.setFontOverline( format&Format::Overline );
    }
    cursor.setCharFormat( charFormat );

    // disable updates if locked
    if( locked_ ) setUpdatesEnabled( false );

    // insert text
    cursor.insertText( text );

    // update
    if( locked_ ) setUpdatesEnabled( true );
    else {

        setTextCursor( cursor );
        ensureCursorVisible();

    }

    return;

}

//______________________________________________________
void LogWidget::keyPressEvent( QKeyEvent* event )
{

    switch( event->key() )
    {
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        locked_ = verticalScrollBar()->value() != verticalScrollBar()->maximum();
        break;

        default: break;
    }

    return TextEditor::keyPressEvent( event );

}

//______________________________________________________
void LogWidget::wheelEvent( QWheelEvent* event )
{
    locked_ = verticalScrollBar()->value() != verticalScrollBar()->maximum();
    return TextEditor::wheelEvent( event );
}

//______________________________________________________
bool LogWidget::_toggleWrapMode( bool value )
{
    Debug::Throw( "LogWidget::_toggleWrapMode.\n" );
    if( _hasOptionName() ) XmlOptions::get().set<bool>( _wrapOptionName(), value );
    return TextEditor::_toggleWrapMode( value );
}

//______________________________________________________
void LogWidget::_verticalScrollBarMoved( int value )
{ locked_ = value != verticalScrollBar()->maximum(); }
