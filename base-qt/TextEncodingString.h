#ifndef TextEncodingString_h
#define TextEncodingString_h

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

#include <QList>
#include <QString>

//! model content
class TextEncodingString: public QString
{
    public:

    //! constructor
    TextEncodingString( void )
    {}

    //! constructor
    TextEncodingString( const QByteArray& other ):
        QString( other )
        { parse(); }

    //! parse string into two lists of digits and text chunks
    void parse( void )
    {
        textSegments_.clear();
        numSegments_.clear();
        int lastPosition = 0;
        int position = 0;
        QRegExp regExp( "(\\d+)" );
        while( ( position = indexOf( regExp, lastPosition ) ) >= 0 )
        {
            textSegments_.append( left( position ) );
            numSegments_.append( regExp.cap(0).toInt() );
            lastPosition = position + regExp.matchedLength();
        }

        textSegments_.append( mid( lastPosition ) );
    }

    //! less than operator
    bool operator < (const TextEncodingString& other ) const
    {
        int textIndex(0);
        int numIndex(0);

        while( true )
        {
            if( textIndex < textSegments_.size() && textIndex < other.textSegments_.size() )
            {
                if( textSegments_[textIndex] != other.textSegments_[textIndex] ) return textSegments_[textIndex] < other.textSegments_[textIndex];
                else textIndex++;
            } else return textSegments_.size() < other.textSegments_.size();

            if( numIndex < numSegments_.size() && numIndex < other.numSegments_.size() )
            {
                if( numSegments_[numIndex] != other.numSegments_[numIndex] ) return numSegments_[numIndex] < other.numSegments_[numIndex];
                else numIndex++;
            }
        }

        return true;
    }

    private:

    QList<QString> textSegments_;
    QList<int> numSegments_;

};

#endif