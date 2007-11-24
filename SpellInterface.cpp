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
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file SpellInterface.cpp
  \brief spell checker interface
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "Debug.h"
#include "SpellInterface.h"
#include "Util.h"
#include "XmlOptions.h"

using namespace std;
using namespace SPELLCHECK;

//_______________________________________________
const string SpellInterface::NO_FILTER = "none";

//_______________________________________________
SpellInterface::SpellInterface( void ):
  Counter( "SpellInterface" ),
  text_(""),
  checked_text_(""),
  begin_(0),
  end_(0),
  position_(0),
  offset_(0),
  spell_config_( new_aspell_config() ),
  spell_checker_( 0 ),
  document_checker_( 0 )
{
  Debug::Throw( "SpellInterface::SpellInterface.\n" );

  // load dictionaries and filters
  _loadDictionaries();
  _loadFilters();
  _resetSpellChecker();
  
  // set default filter
  setFilter( NO_FILTER );
  
  Debug::Throw( "SpellInterface::SpellInterface - done.\n" );
    
}

//__________________________________________
SpellInterface::~SpellInterface( void )
{
  Debug::Throw( "SpellInterface::~SpellInterface.\n" );
  saveWordList();
  if( spell_config_ ) delete_aspell_config( spell_config_ );
  if( document_checker_ ) delete_aspell_document_checker( document_checker_ );
  if( spell_checker_ ) delete_aspell_speller( spell_checker_ );
  Debug::Throw( "SpellInterface::~SpellInterface - done.\n" );
  
}

//____________________________________________________
bool SpellInterface::setDictionary( const std::string& dictionary )
{

  Debug::Throw( "SpellInterface::setDictionary.\n" );
  if( !spell_config_ )
  {
    error_ = "invalid aspell configuration";
    return false;
  }

  // check dictionary
  if( !hasDictionary( dictionary ) )
  {
    error_ = "invalid dictionary";
    return false;
  }

  // save previous personal dictionary
  if( spell_checker_ ) aspell_speller_save_all_word_lists( spell_checker_ );

  // update main dictionary
  aspell_config_replace(spell_config_, "lang", dictionary.c_str() );
  dictionary_ = dictionary;

  // update personal dictionary
  string personal_dictionary = Util::env( "HOME" ) + "/.aspell." + dictionary + ".pws";
  aspell_config_replace(spell_config_, "personal", personal_dictionary.c_str() );

  // reset
  return _reset();

}

//____________________________________________________
bool SpellInterface::setFilter( const std::string& filter )
{

  Debug::Throw( "SpellInterface::setFilter.\n" );
  if( !spell_config_ ) {
    error_ = "invalid aspell configuration";
    return false;
  }

  // check dictionary
  if( !hasFilter( filter ) )
  {
    error_ = "invalid filter";
    return false;
  }

  // update filter
  aspell_config_replace(spell_config_, "mode", filter.c_str() );
  filter_ = filter;

  // reset SpellChecker
  return _reset();

}

//__________________________________________
bool SpellInterface::setText(
    const string& text,
    const unsigned int &begin,
    const unsigned int &end )
{
  Debug::Throw( "SpellInterface::setText.\n" );

  // check text
  if( text.empty() ) 
  {
    Debug::Throw( "SpellInterface::setText - empty.\n" );  
    return true;
  }
  
  // store text
  text_ = text;
  checked_text_ = text;

  // store begin/end position
  begin_ = begin;
  end_ = end;

  // check limits
  if( begin_ > text_.size() ) throw runtime_error( DESCRIPTION( "invalid begin_ index" ) );
  if( end_ > text_.size() ) throw runtime_error( DESCRIPTION( "invalid end_ index" ) );

  end_ += offset_;
  position_ = 0;
  offset_ = 0;
  aspell_document_checker_reset( document_checker_ );
  aspell_document_checker_process(document_checker_, text_.substr( begin_, end_-begin_).c_str(), -1);
  
  return true;

}


//____________________________________________________
bool SpellInterface::addWord( const string& word )
{
  Debug::Throw( "SpellInterface::addWord.\n" );

  // check word
  if( word.empty() ) return false;

  // check spellchecker
  if( !spell_checker_ ) {
    error_ = "invalid aspell checker";
    return false;
  }

  // retrieve word from editor
  aspell_speller_add_to_personal(spell_checker_, word.c_str(), -1);
  return true;

}

//____________________________________________________
bool SpellInterface::replace( const std::string& word )
{

  Debug::Throw( "SpellInterface::replace.\n" );

  // check current word
  if( !word_.size() )
  {
    error_ = "invalid word to be replaced";
    return false;
  }

  // check spell checker
  if( !spell_checker_ ) {
    error_ = "invalid aspell checker";
    return false;
  }


  // inform spellchecker of the replacement
  aspell_speller_store_replacement(
      spell_checker_,
      word_.c_str(), -1,
      word.c_str(), -1 );

  // update checked text
  checked_text_.replace( begin_+position_+offset_, word_.size(), word );
  offset_ += word.size() - word_.size();
  return true;

}

//____________________________________________________
bool SpellInterface::nextWord( void )
{

  // check filter 
  if( filter_.empty() )
  {
    error_ = "no filter set";
    return false;
  }
    
  // check filter 
  if( dictionary_.empty() )
  {
    error_ = "no dictionary set";
    return false;
  }
    
  // check text size
  if( !text_.size() )
  {
    word_ = "";
    error_ = "no text to check";
    return false;
  }

  // check document checker
  if( !document_checker_ ) {
    error_ = "invalid aspell document checker";
    return false;
  }

  AspellToken token;
  while( token = aspell_document_checker_next_misspelling( document_checker_ ), token.len )
  {

    // update position
    position_ = token.offset;

    // retrieve word
    word_ = text_.substr( begin_+position_, token.len );
    return true;
  }

  // spelling completed
  word_ = "";
  position_ = end_;

  return true;

}

//____________________________________________________
vector< string > SpellInterface::suggestions( const std::string& word ) const
{

  Debug::Throw( "SpellInterface::suggestions.\n" );

  // check spell checker
  vector<string> out;
  if( !spell_checker_ ) {
    Debug::Throw(0, "SpellInterface::suggestions - no spell checker" );
    return out;
  }

  const AspellWordList * suggestions( aspell_speller_suggest( spell_checker_, word.c_str(), -1 ) );
  AspellStringEnumeration * elements( aspell_word_list_elements( suggestions ) );

  const char * suggestion( 0 );
  while( ( suggestion = aspell_string_enumeration_next( elements ) ) )
  out.push_back( suggestion );

  delete_aspell_string_enumeration( elements );
  return out;

}

//____________________________________________________
void SpellInterface::saveWordList( void )
{
  Debug::Throw( "SpellInterface::saveWordList.\n" );
  if( spell_checker_ ) aspell_speller_save_all_word_lists( spell_checker_ );
}


//____________________________________________________
bool SpellInterface::reset( void )
{
  Debug::Throw( "SpellInterface::reset.\n" );

  // reinitialize begin and end position
  begin_ = 0;
  end_ = text_.size();

  // reset
  return _reset();

}


//____________________________________________________
void SpellInterface::_loadDictionaries( void )
{

  Debug::Throw( "SpellInterface::_loadDictionaries.\n" );

  dictionaries_.clear();
  if( !spell_config_ ) return;

  const AspellDictInfoList * dictionaries( get_aspell_dict_info_list( spell_config_ ) );
  AspellDictInfoEnumeration *elements( aspell_dict_info_list_elements( dictionaries ) );
  const AspellDictInfo* dictionary( 0 );

  while( ( dictionary = aspell_dict_info_enumeration_next( elements ) ) )
  if( dictionary->code ) dictionaries_.insert( dictionary->code );
  delete_aspell_dict_info_enumeration( elements );
  return;

}

//____________________________________________________
void SpellInterface::_loadFilters( void )
{
  Debug::Throw( "SpellInterface::_loadFilters.\n" );

  filters_.clear();
  filters_.insert( NO_FILTER );

  string command( XmlOptions::get().raw("ASPELL") + " dump modes" );
  FILE *tmp = popen( command.c_str(), "r" );
  static const int linesize( 128 );
  char buf[linesize];
  while( fgets( buf, linesize, tmp ) ) 
  {

    if( !( buf && strlen( buf ) ) ) continue;
    istringstream in( buf );
    string mode;
    in >> mode;
    if( !(in.rdstate() & ios::failbit ) && mode.size() )
    filters_.insert( mode );
  }

  pclose( tmp );
  return;

}

//____________________________________________________
bool SpellInterface::_reset( void )
{

  Debug::Throw( "SpellInterface::_reset\n" );

  // check config
  if( !spell_config_ ) {
    error_ = "invalid aspell configuration";
    return false;
  }

  // delete document checker
  if( document_checker_ ) delete_aspell_document_checker( document_checker_ );
  document_checker_ = 0;

  //!reset spell checker
  if( !_resetSpellChecker() ) 
  {
    Debug::Throw( "SpellInterface::_reset - _resetSpellChecker failed.\n" );
    return false;
  }
  
  // recreate document checker
  AspellCanHaveError* err = new_aspell_document_checker(spell_checker_);
  if( aspell_error_number(err) )
  {
    error_ = aspell_error_message( err );
    Debug::Throw( "SpellInterface::_reset - unable to create document_checker.\n" ); 
    return false;
  } else document_checker_ = to_aspell_document_checker(err);

  // assign text if any
  if( checked_text_.size() )
  {
    Debug::Throw( "SpellInterface::_reset - assigning text.\n" );  
    text_ = checked_text_;
    end_ += offset_;
    position_ = 0;
    offset_ = 0;
    aspell_document_checker_reset( document_checker_ );
    aspell_document_checker_process(document_checker_, text_.substr( begin_, end_-begin_).c_str(), -1);
    Debug::Throw( "SpellInterface::_reset - assigning text, done.\n" );  
  }

  Debug::Throw( "SpellInterface::_reset - done.\n" );   
  return true;

}

//____________________________________________________
bool SpellInterface::_resetSpellChecker( void )
{

  Debug::Throw( "SpellInterface::_resetSpellChecker\n" );

  // check config
  if( !spell_config_ ) {
    Debug::Throw( "SpellInterface::_resetSpellChecker - invalid aspell configuration.\n" ); 
    error_ = "invalid aspell configuration";
    return false;
  }

  // delete spell checker
  if( spell_checker_ ) delete_aspell_speller( spell_checker_ );
  spell_checker_ = 0;

  // recreate spell checker
  AspellCanHaveError* err = new_aspell_speller(spell_config_);
  if( aspell_error_number(err) )
  {
    Debug::Throw( "SpellInterface::_resetSpellChecker - failed to create speller.\n" );  
    error_ = aspell_error_message( err );
    return false;
  } else spell_checker_ = to_aspell_speller(err);

  return true;

};
