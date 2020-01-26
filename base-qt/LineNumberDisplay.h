#ifndef LineNumberDisplay_h
#define LineNumberDisplay_h

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
#include "Debug.h"

#include <QColor>
#include <QFont>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTextBlock>
#include <QObject>
#include <QList>

class TextEditor;

//* display line number of a text editor
class LineNumberDisplay: public QObject, private Base::Counter<LineNumberDisplay>
{

    //* Qt meta object
    Q_OBJECT

    public:

    //* constructor
    explicit LineNumberDisplay(TextEditor*);

    //* synchronization
    void synchronize( LineNumberDisplay* );

    //* width
    bool updateWidth( int );

    //* width
    int width() const
    { return width_; }

    //* clear everything
    void clear();

    //* paint
    void paint( QPainter& );

    public Q_SLOTS:

    //* need update
    void needUpdate()
    { needsUpdate_ = true; }

    private Q_SLOTS:

    //* contents changed
    void _contentsChanged();

    //* block count changed
    void _blockCountChanged();

    private:

    //* map block number and position
    class LineNumberData final
    {

        public:

        //* list of data
        using List = QList<LineNumberData>;

        //* constructor
        explicit LineNumberData( int id = 0, int lineNumber = 0, int cursor = 0):
            id_( id ),
            lineNumber_( lineNumber ),
            cursor_( cursor ),
            position_( -1 ),
            valid_( false )
        {}

        //* id
        int id() const
        { return id_; }

        //* line number
        int lineNumber() const
        { return lineNumber_; }

        //* y
        int cursor() const
        { return cursor_; }

        //* position
        void setPosition( int position )
        {
            valid_ = (position >= 0);
            position_ = position;
        }

        //* position
        int position() const
        { return position_; }

        //* validity
        bool isValid() const
        { return valid_; }

        private:

        //* block id
        int id_ = 0;

        //* line number
        int lineNumber_ = 0;

        //* position
        int cursor_ = 0;

        //* position
        int position_ = -1;

        //* validity
        bool valid_ = false;

        //* equal to operator
        friend bool operator == (const LineNumberData& first, const LineNumberData& second )
        { return first.id() == second.id(); }

    };

    //* update line number data
    void _updateLineNumberData();

    //* update invalid data
    void _updateLineNumberData( QTextBlock&, int&, LineNumberData& ) const;

    //* associated editor
    TextEditor* editor_ = nullptr;

    //* true when line number data update is needed
    bool needsUpdate_ = true;

    //* width
    int width_ = 0;

    //* line number data
    LineNumberData::List lineNumberData_;

    //* current block data
    LineNumberData currentBlockData_;

};

#endif
