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
#include <QRegularExpression>
#include <QString>

//* model content
class TextEncodingString final
{
    public:

    //* universal constructor
    template<typename... Args>
    explicit TextEncodingString(Args&&... args):
        value_( std::forward<Args>(args)... )
    { parse(); }

    //* accessor
    const QString& get() const { return value_; }

    //* parse string into two lists of digits and text chunks
    void parse()
    {
        textSegments_.clear();
        numSegments_.clear();
        int position = 0;
        static const QRegularExpression regexp( QStringLiteral("(\\d+)") );
        auto iter = regexp.globalMatch( value_ );
        while( iter.hasNext() )
        {
            const auto match( iter.next() );
            if( match.capturedStart() > position ) textSegments_.append( value_.mid( position, match.capturedStart() - position ) );
            numSegments_.append( match.captured().toInt() );
            position = match.capturedEnd();
        }
        textSegments_.append( value_.mid( position ) );
    }

    private:

    QString value_;
    QStringList textSegments_;
    QList<int> numSegments_;

    //* less than operator
    friend bool operator < (const TextEncodingString& first, const TextEncodingString& second )
    {
        int textIndex(0);
        int numIndex(0);

        forever
        {
            if( textIndex < first.textSegments_.size() && textIndex < second.textSegments_.size() )
            {
                if( first.textSegments_[textIndex] != second.textSegments_[textIndex] ) return first.textSegments_[textIndex] < second.textSegments_[textIndex];
                else ++textIndex;
            } else return first.textSegments_.size() < second.textSegments_.size();

            if( numIndex < first.numSegments_.size() && numIndex < second.numSegments_.size() )
            {
                if( first.numSegments_[numIndex] != second.numSegments_[numIndex] ) return first.numSegments_[numIndex] < second.numSegments_[numIndex];
                else ++numIndex;
            }
        }

        return true;
    }

};

//* equal to operator
inline bool operator == (const TextEncodingString& lhs, const TextEncodingString& rhs)
{ return lhs.get() == rhs.get(); }

//* specialized copy constructor
template<> TextEncodingString::TextEncodingString(TextEncodingString& );
template<> TextEncodingString::TextEncodingString(const TextEncodingString& );

#endif
