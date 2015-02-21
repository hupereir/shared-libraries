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

        //! constructor
        Position( int position, const TexString::Conversion conversion ):
            position_(position),
            conversion_(conversion)
        {}

        int position_;
        TexString::Conversion conversion_;

        //! less than operator
        bool operator < (const Position& other ) const
        { return position_ < other.position_; }

        //! list
        using List=QList<Position>;

    };

    //____________________________________________________________________________
    SpellParser::SpellParser( void ):
        Counter( "SpellParser" ),
        enabled_( false ),
        fontFormat_( Format::Default )
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
        if( interface().filter() == SpellInterface::FilterTexWithNoAccents )
        {

            // apply conversions
            text = TexString( text ).toTextAccents();

            // build positions list
            foreach( const TexString::Conversion& conversion, TexString::conversions() )
            {
                int position(0);
                while( (position = constText.indexOf( conversion.second, position )) >= 0 )
                {
                    positions.append( Position( position, conversion ) );
                    position+= conversion.second.size();
                }
            }

            // sort positions
            std::sort( positions.begin(), positions.end() );

        }

        // retrieve misspelled words
        interface().setText( text );
        Word::Set words;
        forever
        {
            interface().nextWord();
            QString word( interface().word() );
            int wordPosition( interface().position() );
            if( word.isEmpty() ) break;
            if( interface().isWordIgnored( word ) ) continue;

            // apply offset
            if( interface().filter() == SpellInterface::FilterTexWithNoAccents && !positions.empty() )
            {

                // update position
                int offset = 0;
                foreach( const Position& position, positions )
                {
                    if( position.position_ - offset <= wordPosition ) offset += (position.conversion_.second.size() - position.conversion_.first.size() );
                    else break;
                }

                wordPosition += offset;

                // update word
                foreach( const Position& position, positions )
                {
                    if( position.position_ < wordPosition ) continue;
                    else if( position.position_ - wordPosition > word.size() ) break;
                    else word.replace( position.position_ - wordPosition, position.conversion_.first.size(), position.conversion_.second );

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
