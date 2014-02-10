#ifndef Margins_h
#define Margins_h
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

#include <QTextStream>
#include <QPoint>
#include <QRect>

namespace Base
{
    class Margins
    {

        public:

        //! constructor
        Margins( int margin = 0 ):
            left_(margin),
            top_(margin),
            right_(margin),
            bottom_(margin)
        {}

        //! add to operator
        Margins& operator += (const Margins& other )
        {
            left_ += other.left_;
            top_ += other.top_;
            right_ += other.right_;
            bottom_ += other.bottom_;
            return *this;
        }

        //! remove from operator
        Margins& operator -= (const Margins& other )
        {
            left_ -= other.left_;
            top_ -= other.top_;
            right_ -= other.right_;
            bottom_ -= other.bottom_;
            return *this;
        }

        //!@name accessors
        //@{

        //! dimensions
        int left( void ) const
        { return left_; }

        //! dimensions
        int top( void ) const
        { return top_; }

        //! dimensions
        int right( void ) const
        { return right_; }

        //! dimensions
        int bottom( void ) const
        { return bottom_; }

        //! true if null
        bool isNull( void ) const
        {
            return
                left_ == 0 &&
                top_ == 0  &&
                right_ == 0 &&
                bottom_ == 0;
        }

        //! width
        int width( void ) const
        { return left_+right_; }

        //! height
        int height( void ) const
        { return top_+bottom_; }

        //! return adjusted rect
        QRect adjustedRect( const QRect& source ) const
        { return source.adjusted( left_, top_, -right_, -bottom_ ); }

        //! top left point
        QPoint topLeft( void ) const
        { return QPoint( left_, top_ ); }

        //@}

        //!@name modifiers
        //@{

        //! clear
        void clear( void )
        {
            left_ = 0;
            top_ = 0;
            right_ = 0;
            bottom_ = 0;
        }

        //! dimensions
        void setLeft( int value )
        { left_ = value; }

        //! dimensions
        void setTop( int value )
        { top_ = value; }

        //! dimensions
        void setRight( int value )
        { right_ = value; }

        //! dimensions
        void setBottom( int value )
        { bottom_ = value; }

        //@}

        private:

        int left_;
        int top_;
        int right_;
        int bottom_;

        //! streamer
        friend QTextStream& operator << ( QTextStream& out, const Margins& margins )
        {
            out << "(" << margins.left_ << "," << margins.top_ << "," << margins.right_ << "," << margins.bottom_ << ")";
            return out;
        }
    };

};

#endif
