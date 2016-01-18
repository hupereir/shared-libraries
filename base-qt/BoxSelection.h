#ifndef BoxSelection_h
#define BoxSelection_h

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

#include "Counter.h"

#include <QClipboard>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QTextCursor>

class TextEditor;

//* handles box selection
class BoxSelection: public Counter
{

    public:

    //* mime type
    static const QString mimeType;

    //* state
    enum State
    {
        SelectionEmpty,
        SelectionStarted,
        SelectionFinished
    };

    //* constructor
    BoxSelection( TextEditor* parent );

    //* synchronize two box selection
    void synchronize( const BoxSelection& selection );

    //*@ accessors
    //@{

    //* configuration update
    void updateConfiguration( void );

    //* try enable box selection
    bool checkEnabled( void );

    //* enability
    bool isEnabled( void ) const
    { return enabled_; }

    //* forgeround color
    const QColor& color( void ) const
    { return color_; }

    //* background brush
    const QBrush& brush( void ) const
    { return brush_; }

    //* state
    const State& state( void ) const
    { return state_; }

    //* start point
    const QPoint &begin() const
    { return begin_; }

    //* end point
    const QPoint &end() const
    { return end_; }

    //* rect
    const QRect& rect( void ) const
    { return rect_; }

    //@}

    //*@name modifiers
    //@{

    //* start
    bool start( QPoint point );

    //* draw
    bool update( QPoint point );

    //* finish
    bool finish( QPoint point );

    //* clear
    bool clear( void );

    //@}

    //* store selection cursors and  selection columns
    class CursorList: public  QList<QTextCursor>, public Counter
    {

        public:

        //* constructor
        CursorList( const int& first_column = 0, const int& columns = 0 ):
            Counter( "CursorList" ),
            firstColumn_( first_column ),
            columns_( columns )
        {}

        //* first column
        /*! it is used for lines that are entirely outside of the existing blocks */
        const int& firstColumn( void ) const
        { return firstColumn_; }

        //* number of columns in the selection
        const int& columnCount( void ) const
        { return columns_; }

        private:

        //* first column
        /*! it is used for lines that are entirely outside of the existing blocks */
        int firstColumn_;

        //* selection columns
        int columns_;

    };

    //*@name selection manipulation
    //@{

    //* retrieve list of cursors matching the selection
    /*! they are used for cut, copy, paste and searching */
    const CursorList& cursorList( void ) const
    { return cursors_; }

    //* copy selection into a string
    QString toString( void ) const;

    //* update from string
    bool fromString( QString );

    //* copy selection content to clipboard
    bool toClipboard( const QClipboard::Mode& ) const;

    //* copy clipboard content to boxSelection
    bool fromClipboard( const QClipboard::Mode& );

    //* remove selected text
    bool removeSelectedText( void ) const;

    //* convert selected text to upperCase
    bool toUpper( void );

    //* convert selected text to upperCase
    bool toLower( void );

    //* apply char format to selection
    bool mergeCharFormat( const QTextCharFormat& ) const;

    //@}

    private:

    //* update repaiint rect
    void _updateRect( void );

    //* store selected text
    void _store( void );

    //* parent editor
    TextEditor* parent_ = nullptr;

    //* @name attributes
    //@{

    //* true if box selection is enabled
    /*! box selection is enabled for fixed pitch fonts only */
    bool enabled_ = false;

    //* selection color
    QColor color_;

    //* selection brush
    QBrush brush_;

    //* font width
    int fontWidth_ = 0;

    //* font height
    int fontHeight_ = 0;

    //@}

    //*@name current selection properties

    //* state
    State state_ = SelectionEmpty;

    //* start point
    QPoint begin_;

    //* end point
    QPoint end_;

    //* cursor point
    /*!
    it is used to tell the editor where to place the cursor
    when highlighting */
    QPoint cursor_;

    //* max rectangle
    QRect rect_;

    //* list of selection cursors
    CursorList cursors_;

};

#endif
