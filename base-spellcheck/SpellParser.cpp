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

#include "SpellInterface.h"
#include "SpellParser.h"
#include "TexString.h"
#include "Word.h"

#include <algorithm>

namespace SpellCheck
{

    //! latex conversion
    class Position
    {

        public:

        //* constructor
        explicit Position( int position, const TexString::Conversion conversion ):
            position_(position),
            conversion_(conversion)
        {}

        //*@name accessors
        //@{

        //* position
        int position() const { return position_; }

        //* conversion
        TexString::Conversion conversion() const { return conversion_; }

        //* list
        using List=QList<Position>;

        private:

        int position_;

        TexString::Conversion conversion_;

    };

    //* less than operator
    bool operator < (const Position& first, const Position& second )
    { return first.position() < second.position(); }

    //____________________________________________________________________________
    SpellParser::SpellParser():
        Counter( "SpellParser" )
    {  Debug::Throw( "SpellParser::SpellParser.\n" ); }

    //____________________________________________________________________________
    Word::Set SpellParser::parse( const QString& constText )
    {

        Debug::Throw( "SpellParser::Parse.\n" );

        // local copy
        QString text( constText );

        // check if enabled
        if( !enabled_ ) return ( Word::Set() );

        Position::List positions;
        if( interface_.filter() == SpellInterface::FilterTexWithNoAccents )
        {

            // apply conversions
            text = TexString( text ).toTextAccents();

            // build positions list
            for( const auto& conversion:TexString::conversions() )
            {
                for( int position = 0; (position = constText.indexOf( conversion.second, position )) >= 0; position+= conversion.second.size() )
                { positions.append( Position( position, conversion ) ); }
            }

            // sort positions
            std::sort( positions.begin(), positions.end() );

        }

        // retrieve misspelled words
        interface_.setText( text );
        Word::Set words;
        forever
        {
            interface_.nextWord();
            auto word( interface_.word() );
            int wordPosition( interface_.position() );
            if( word.isEmpty() ) break;
            if( interface_.isWordIgnored( word ) ) continue;

            // apply offset
            if( interface_.filter() == SpellInterface::FilterTexWithNoAccents && !positions.empty() )
            {

                // update position
                int offset = 0;
                for( const auto& position:positions )
                {
                    if( position.position() - offset <= wordPosition ) offset += (position.conversion().second.size() - position.conversion().first.size() );
                    else break;
                }

                wordPosition += offset;

                // update word
                for( const auto& position:positions )
                {
                    if( position.position() < wordPosition ) continue;
                    else if( position.position() - wordPosition > word.size() ) break;
                    else word.replace( position.position() - wordPosition, position.conversion().first.size(), position.conversion().second );
                }

            }

            Debug::Throw() << "SpellParser::highlightParagraph - new word: " << word << " position: " << wordPosition << endl;
            words.insert( Word( word, wordPosition ) );

        }

        return words;
    }

    //___________________________________________________________
    bool SpellParser::setColor( const QColor& color )
    {
        Debug::Throw( "SpellParser::setColor.\n" );
        if( color == color_ || !color.isValid() ) return false;
        else {
            color_ = color;
            return true;
        }
    }

    //___________________________________________________________
    bool SpellParser::setFontFormat( Format::TextFormatFlags format )
    {

        Debug::Throw( "SpellParser::setFontFormat.\n" );
        if( fontFormat_ == format ) return false;
        else {
            fontFormat_ = format;
            return true;
        }

    }
}
