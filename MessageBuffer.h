#ifndef MessageBuffer_h
#define MessageBuffer_h

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

#include <QString>

//* buffer class and current position
class MessageBuffer
{
    public:

    //* constructor
    MessageBuffer( void )
    {}

    //* clear
    void clear( void )
    {
        text_ = QString();
        position_ = 0;
    }

    //* append
    void append( const QString& buffer )
    { text_ += buffer; }

    //* move to position
    void flush( void )
    { flush( position() ); }

    //* move to new position
    void flush( const int& position )
    {
        text_ = text_.mid( position );
        position_ = 0;
    }

    //* text
    const QString& text( void ) const
    { return text_; }

    //* text
    QString& text( void )
    { return text_; }

    //* position
    const int& position( void ) const
    { return position_; }

    private:

    //* text
    QString text_;

    //* last parsed position
    int position_ = 0;

};
#endif
