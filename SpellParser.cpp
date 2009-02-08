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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
  \file SpellParser.cpp
  \brief highlight text based on misspelled words
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/



#include "SpellInterface.h"
#include "SpellParser.h"
#include "Word.h"

using namespace std;
using namespace SPELLCHECK;

//____________________________________________________________________________
SpellParser::SpellParser( void ):
    Counter( "SpellParser" ),
    enabled_( false ),
    font_format_( FORMAT::DEFAULT )
{  Debug::Throw( "SpellParser::SpellParser.\n" ); }

//____________________________________________________________________________
Word::Set SpellParser::parse( const QString& text )
{
  
  Debug::Throw( "SpellParser::Parse.\n" );

  // check if enabled
  if( !enabled_ ) return ( Word::Set() );
  
  // retrieve misspelled words
  interface().setText( text );
  Word::Set words;
  while(1)
  {
    interface().nextWord();
    QString word( interface().word() );
    int position( interface().position() );
    if( word.isEmpty() ) break;
    if( interface().isWordIgnored( word ) ) continue;
    
    Debug::Throw() << "SpellParser::highlightParagraph - new word: " << word << " position: " << position << endl;
    words.insert( Word( word, position ) );
    
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
bool SpellParser::setFontFormat( const unsigned int& format )
{ 
      
  Debug::Throw( "SpellParser::setFontFormat.\n" );
  if( font_format_ == format ) return false;
  else {
    font_format_ = format;
    return true;
  }
  
}
