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

#ifndef SpellInterface_h
#define SpellInterface_h

/*!
  \file SpellInterface.h
  \brief spell checker interface
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <aspell.h>
#include <list>
#include <set>
#include <string>

#include "Counter.h"
#include "Exception.h"

namespace SPELLCHECK
{
  //! spell checker interface
  class SpellInterface: public Counter
  {  
    
    public:
    
    //! constructor
    SpellInterface( void );    
    
    //! destructor
    ~SpellInterface( void );
    
    //! no filter
    static const std::string NO_FILTER;
    
    //!@name dictionary
    //@{
    
    //! reset dictionary from options
    static bool resetDictionary( void );
    
    //! get list of available dictionaries
    const std::set< std::string >& dictionaries( void ) const
    { return dictionaries_; }
    
    //! current dictionary
    const std::string dictionary( void ) const
    { return dictionary_; }

    //! select dictionary
    /*! returns false in case of error */
    bool setDictionary( const std::string& dictionary );
    
    //@}
    
    //!@name filter
    //@{
    
    //! reset filter from options
    static bool resetFilter( void );

    //! get list of available filters
    const std::set< std::string >& filters( void ) const
    { return filters_; }

    //! current filter
    const std::string filter( void ) const
    { return filter_; }

    //! select filter
    /*! returns false in case of error */
    bool setFilter( const std::string& filter );
    
    //@}
    
    //!@name ignored words
    //@{
    
    //! add word to static list
    void ignoreWord( const std::string& word )
    { ignored_words_.insert( word ); }
    
    //! returns true if word is ignored
    bool isWordIgnored( const std::string& word )
    { return ignored_words_.find( word ) != ignored_words_.end(); }
    
    //! set list of ignored words
    void setIgnoredWords( const std::set<std::string> words )
    { ignored_words_ = words; }
    
    //! merge list of ignored words
    void mergeIgnoredWords( const std::set<std::string> words )
    { ignored_words_.insert( words.begin(), words.end() ); }
    
    //! clear ignored words
    void clearIgnoredWords( void )
    { ignored_words_.clear(); }
    
    //! get list of ignored words
    const std::set< std::string > ignoredWords( void )
    { return ignored_words_; }
    
    //!@name text
    //@{
       
    //! set text to be checked
    bool setText( const std::string& text )
    { return setText( text, 0, text.size() ); }
    
    //! set text to be checked
    /*!
      returns false on error
      \param text the text in which selection is to be checked
      \param begin selection begin position
      \param end selection end position in the text
    */  
    bool setText( const std::string& text, const unsigned int& begin, const unsigned int& end );

    //! retrieve original text
    const std::string& text( void ) const
    { return text_; }
    
    //! retrieve checked text
    const std::string& checkedText( void ) const
    { return checked_text_; }
    
    //@}
    
    //! retrieve error
    const std::string& error( void ) const
    { return error_; }
    
    
    //! retrieve current word being checked
    const std::string& word( void ) const
    { return word_; }
    
    //! retrieve position of current word in original text
    unsigned int position( void ) const
    { return position_+begin_; }
    
    //! retrieve position offset between original text and checked text
    const unsigned int& offset( void ) const
    { return offset_; }
    
    //! add word to dictionary
    /*! return false on error */
    bool addWord( const std::string& word );

    //! add current word to dictionary
    /*! return false on error */
    bool addWord( void )
    { return addWord( word() ); }
        
    //! replace current word with argument in checked text
    bool replace( const std::string& new_word );
    
    //! retrieve next word to be checked
    /*! return empty string when completed */
    bool nextWord( void );
    
    //! retrieve list of suggestions for current word
    std::list< std::string > suggestions( const std::string& word ) const;
    
    //! save word list in personal dictionary
    void saveWordList( void );
    
    //! restart checking
    /*! 
      initial begin/end are reset to the full text 
      return false on error
    */
    bool reset( void );
                
    private:
    
    //! reset dictionary from options
    bool _resetDictionary( void );
  
    //! load available dictionaries
    void _loadDictionaries( void );
    
    //! reset filter from options
    bool _resetFilter( void );
      
    //! load available filters
    void _loadFilters( void );
      
    //! reset Aspell spell/document checker
    /*! returns false on error */
    virtual bool _reset( void );
      
    //! reset Aspell spell checker
    /*! returns false on error */
    virtual bool _resetSpellChecker( void );
  
    //! list of registered interfaces
    static std::set<SpellInterface*> interfaces_;
     
    //! set of available dictionaries
    /*! loaded in constructor */
    std::set<std::string> dictionaries_;    
    
    //! set of available filters
    /*! loaded in constructor */
    std::set<std::string> filters_; 
    
    //! ignored words
    std::set<std::string> ignored_words_;
       
    //! original text to be checked
    std::string text_;
    
    //! text being checked
    std::string checked_text_;
    
    //! current word being checked
    std::string word_;
    
    //! begin position to be checked in text 
    unsigned int begin_;
    
    //! end position to be checked in text
    unsigned int end_;
    
    //! position in started text (counted from begin_)
    unsigned int position_;
    
    //! offset between checked_text_ and text_
    unsigned int offset_;    
    
    //! configuration error
    std::string error_;
    
    //! dictionary
    std::string dictionary_;
    
    //! filter
    std::string filter_;
    
    //! aspell configuration singleton
    AspellConfig* spell_config_;
    
    //! aspell checker
    AspellSpeller* spell_checker_;
    
    //! aspell checker
    AspellDocumentChecker* document_checker_;
  };
    
};  

#endif
