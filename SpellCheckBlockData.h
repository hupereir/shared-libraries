#ifndef SpellCheckBlockData_h
#define SpellCheckBlockData_h

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
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA 02111-1307 USA                           
*                         
*                         
*******************************************************************************/
 
/*!
  \file SpellCheckBlockData.h
  \brief TextBlock data for spellCheck highlighting
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/  

#include "TextBlockData.h"
#include "Word.h"

namespace SPELLCHECK
{
  //! TextBlock data
  class SpellCheckBlockData: public TextBlockData
  {
    
    public: 
    
    //! constructor
    SpellCheckBlockData():
      TextBlockData()
    { Debug::Throw( "SpellCheckBlockData::SpellCheckBlockData.\n" ); }
      
    //! constructor
    SpellCheckBlockData( const TextBlockData& reference ):
      TextBlockData( reference )
    { Debug::Throw( "SpellCheckBlockData::SpellCheckBlockData.\n" ); }
      
    //! constructor
    SpellCheckBlockData( const TextBlockData* pointer ):
      TextBlockData( *pointer )
    { Debug::Throw( "SpellCheckBlockData::SpellCheckBlockData.\n" ); }
 
    //! destructor
    virtual ~SpellCheckBlockData( void )
    { Debug::Throw( "SpellCheckBlockData::~SpellCheckBlockData.\n" ); }
    
    //! active block
    const Word::Set& misspelledWords( void ) const
    { return words_; }
    
    //! active block
    void setMisspelledWords( const Word::Set& words )
    { words_ = words; }
    
    //! return misspelled word matching position, if any
    Word misspelledWord( const int& position ) const;
    
    private:
    
    //! set to true for current block
    Word::Set words_;
    
  };
};

#endif
  
