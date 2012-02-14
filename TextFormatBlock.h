// $Id$
#ifndef _TextFormatBlock_h_
#define _TextFormatBlock_h_

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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "Counter.h"

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

namespace FORMAT
{
    //! text format
    /*!
    starting with Qt4, text blocks are no longer referenced by paragraph and index separately, but
    by an absolute index in document. However one must keep the possibility to handle paragraph id
    and convert them to absolute indexes in order to read old formats generated with Qt3.
    As a result parBegin and parEnd are still available but cannot be modified from outside of the class.
    */
    class TextFormatBlock: public Counter
    {

        public:

        //! constructor
        TextFormatBlock(
            const int& begin = 0, const int& end = 0,
            const unsigned int format = 0, const QString& color = "" ):
            Counter( "TextFormatBlock" ),
            begin_( begin ),
            end_( end ),
            parBegin_(0),
            parEnd_(0),
            format_( format ),
            color_( color )
        {}

        //! less than operator (based on Begin position)
        bool operator < (const TextFormatBlock& format ) const
        { return begin() < format.begin(); }

        //! true if format info is empty
        bool isEmpty( void ) const
        { return begin() == end(); }

        //! begin position
        const int& begin() const
        { return begin_; }

        //! begin position
        int& begin()
        { return begin_; }

        //! end position
        const int& end() const
        { return end_; }

        //! end position
        int& end()
        { return end_; }

        //!@name obsolete paragraph indexes
        //@{

        const int& parBegin() const
        { return parBegin_; }

        //!@name obsolete paragraph indexes
        const int& parEnd() const
        { return parEnd_; }

        //@}

        //! format
        const unsigned int& format() const
        { return format_; }

        //! format
        unsigned int& format()
        { return format_; }

        //! color
        const QString& color() const
        { return color_; }

        //! color
        QString& color()
        { return color_; }

        //! get TextFormatBlock matching a given begin position
        class SamePositionBeginFTor
        {

            public:

            //! constructor
            SamePositionBeginFTor( const int& position ):
                position_( position )
            {}

            //! predicate
            bool operator() (const TextFormatBlock& format ) const
            { return format.begin() == position_; }

            private:

            //! predicted position
            int position_;

        };

        //! get TextFormatBlock matching a given end position
        class SamePositionEndFTor
        {

            public:

            //! constructor
            SamePositionEndFTor( const int& position ):
                position_( position )
            {}

            //! predicate
            bool operator() (const TextFormatBlock& format ) const
            { return format.end() == position_; }

            private:

            //! predicted position
            int position_;

        };

        //! get TextFormatBlock containing a given position
        class ContainsFTor
        {

            public:

            //! constructor
            ContainsFTor( const int& position ):
                position_( position )
            {}

            //! predicate
            bool operator() (const TextFormatBlock& format ) const
            { return position_ >= format.begin() && position_ < format.end(); }

            private:

            //! predicted position
            int position_;

        };


        //! format list
        typedef QVector<FORMAT::TextFormatBlock> List;

        protected:

        //!@name obsolete paragraph indexes
        //@{

        int& _parBegin()
        { return parBegin_; }

        //!@name obsolete paragraph indexes
        int& _parEnd()
        { return parEnd_; }

        //@}

        private:

        //! starting range
        int begin_;

        //! ending range
        int end_;

        //!@name obsolete paragraph indexes
        //@{

        //! begin paragraph
        int parBegin_;

        //! end paragraph
        int parEnd_;

        //@}

        //! format (a bitwise or of the format bits above)
        unsigned int format_;

        //! color
        QString color_;

        //! streamer
        friend QTextStream& operator << ( QTextStream& out, const TextFormatBlock& format )
        {
            out
                << "begin: " << format.begin()
                << " end: " << format.end()
                << " format: " << format.format()
                << " color: " << format.color()
                << endl;
            return out;
        }

        //! streamer
        friend QTextStream& operator << ( QTextStream& out, const TextFormatBlock::List& formats )
        {
            out << "TextFormatList: " << endl;
            for( TextFormatBlock::List::const_iterator iter = formats.begin(); iter != formats.end(); iter++ )
            { out << *iter; }
            out << endl;
            return out;
        }

    };


};

#endif
