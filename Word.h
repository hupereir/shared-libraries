#ifndef _Word_h_
#define _Word_h_

  
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
   \file Word.h
   \brief keep track of misspelled words in text
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <set>
#include "TextPosition.h"

namespace SPELLCHECK
{
  //! keep track of misspelled words in text
  class Word: public std::string
  {
    public:

    //! shortcut for set
    typedef std::set<Word> Set;
        
    //! constructor
    Word( const std::string& word = "", const int& position = 0 ):
        std::string( word ),
        position_( position )
    {}    
    
    //! equal to operator
    bool operator == ( const Word& word ) const 
    { return position_ == word.position_; }
    
    //! less than to operator
    bool operator < ( const Word& word ) const 
    { return position_ < word.position_; }
      
    //! position
    const int& position( void ) const
    { return position_; }
    
    //! true if given position is in the selected word
    bool has( const int& position ) const
    { 
      return 
        position >= position_ && 
        position < position_+static_cast<int>(size()); 
    }
    
    //! returns true if Word is find at position
    class AtPositionFTor
    {
      
      public:
          
      //! constructor
      AtPositionFTor( const int& position ):
          position_( position )
      {}
      
      //! predicate
      bool operator() (const Word& word )
      { return word.has( position_ ); }
      
      private:
      
      //! predicate position
      const int position_;
      
    };
    
    private:
    
    //! position in text
    const int position_;    
    
  };
}

#endif
