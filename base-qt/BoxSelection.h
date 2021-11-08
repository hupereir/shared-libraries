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
#include "base_qt_export.h"

#include <QClipboard>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QTextCursor>

class TextEditor;

//* handles box selection
class BASE_QT_EXPORT BoxSelection final: private Base::Counter<BoxSelection>
{

    public:

    //* mime type
    static const QString mimeType;

    //* state
    enum class State
    {
        Empty,
        Started,
        Finished
    };

    //* constructor
    explicit BoxSelection( TextEditor* parent );

    //*@ accessors
    //@{

    //* enability
    bool isEnabled() const
    { return enabled_; }

    //* empty
    bool empty() const;

    //* forgeround color
    const QColor& color() const
    { return color_; }

    //* background brush
    const QBrush& brush() const
    { return brush_; }

    //* state
    const State& state() const
    { return state_; }

    //* start point
    const QPoint &begin() const
    { return begin_; }

    //* end point
    const QPoint &end() const
    { return end_; }

    //* rect
    const QRect& rect() const
    { return rect_; }

    //@}

    //*@name modifiers
    //@{

    //* synchronize two box selection
    void synchronize( const BoxSelection& );

    //* configuration update
    void updateConfiguration();

    //* try enable box selection
    bool checkEnabled();

    //* start
    bool start( QPoint point );

    //* draw
    bool update( QPoint point );

    //* finish
    bool finish( QPoint point );

    //* clear
    bool clear();

    //@}

    //* store selection cursors and  selection columns
    class BASE_QT_EXPORT CursorList: public  QList<QTextCursor>, private Base::Counter<CursorList>
    {

        public:

        //* constructor
        explicit CursorList( int firstColumn = 0, int columns = 0 ):
            Counter( QStringLiteral("CursorList") ),
            firstColumn_( firstColumn ),
            columns_( columns )
        {}

        //* first column
        /*! it is used for lines that are entirely outside of the existing blocks */
        int firstColumn() const
        { return firstColumn_; }

        //* number of columns in the selection
        int columnCount() const
        { return columns_; }

        private:

        //* first column
        /*! it is used for lines that are entirely outside of the existing blocks */
        int firstColumn_ = 0;

        //* selection columns
        int columns_ = 0;

    };

    //*@name selection manipulation
    //@{

    //* retrieve list of cursors matching the selection
    /*! they are used for cut, copy, paste and searching */
    const CursorList& cursorList() const
    { return cursors_; }

    //* copy selection into a string
    QString toString() const;

    //* update from string
    bool fromString( QString );

    //* copy selection content to clipboard
    bool toClipboard( QClipboard::Mode  ) const;

    //* copy clipboard content to boxSelection
    bool fromClipboard( QClipboard::Mode  );

    //* remove selected text
    bool removeSelectedText() const;

    //* convert selected text to upperCase
    bool toUpper();

    //* convert selected text to upperCase
    bool toLower();

    //* apply char format to selection
    bool mergeCharFormat( const QTextCharFormat& ) const;

    //@}

    private:

    //* update repaiint rect
    void _updateRect();

    //* store selected text
    void _store();

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
    State state_ = State::Empty;

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
