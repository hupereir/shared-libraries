#ifndef LineNumberDisplay_h
#define LineNumberDisplay_h

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

#include "Counter.h"
#include "Debug.h"

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QTextBlock>
#include <QtCore/QObject>
#include <QtCore/QList>

class TextEditor;

//! display line number of a text editor
class LineNumberDisplay: public QObject, public Counter
{

    //! Qt meta object
    Q_OBJECT

        public:

        //! constructor
        LineNumberDisplay(TextEditor*);

    //! destructor
    virtual ~LineNumberDisplay();

    //! synchronization
    void synchronize( LineNumberDisplay* );

    //! width
    virtual bool updateWidth( const int& );

    //! width
    virtual const int& width( void ) const
    { return width_; }

    //! clear everything
    void clear();

    //! paint
    virtual void paint( QPainter& );

    public slots:

    //! need update
    void needUpdate( void )
    { needsUpdate_ = true; }

    private slots:

    //! contents changed
    void _contentsChanged( void );

    //! block count changed
    void _blockCountChanged( void );

    private:

    //! editor
    TextEditor& _editor( void ) const
    { return *editor_; }

    //! map block number and position
    class LineNumberData
    {

        public:

        //! list of data
        typedef QList<LineNumberData> List;

        //! constructor
        LineNumberData( const unsigned int& id = 0, const unsigned int& line_number = 0, const int& cursor = 0):
            id_( id ),
            lineNumber_( line_number ),
            cursor_( cursor ),
            position_( -1 ),
            valid_( false )
        {}

        //! equal to operator
        bool operator == (const LineNumberData& data )
        { return id() == data.id(); }

        //! id
        const unsigned int& id( void ) const
        { return id_; }

        //! line number
        const unsigned int& lineNumber( void ) const
        { return lineNumber_; }

        //! y
        const int& cursor( void ) const
        { return cursor_; }

        //! position
        void setPosition( const int& position )
        {
            valid_ = (position >= 0);
            position_ = position;
        }

        //! position
        const int& position( void ) const
        { return position_; }

        //! validity
        const bool& isValid( void ) const
        { return valid_; }

        private:

        //! block id
        unsigned int id_;

        //! line number
        unsigned int lineNumber_;

        //! position
        int cursor_;

        //! position
        int position_;

        //! validity
        bool valid_;

    };

    //! update line number data
    void _updateLineNumberData( void );

    //! update invalid data
    void _updateLineNumberData( QTextBlock&, unsigned int&, LineNumberData& ) const;

    //! associated editor
    TextEditor* editor_;

    //! true when line number data update is needed
    bool needsUpdate_;

    //! width
    int width_;

    //! line number data
    LineNumberData::List lineNumberData_;

    //! current block data
    LineNumberData currentBlockData_;

};

#endif
