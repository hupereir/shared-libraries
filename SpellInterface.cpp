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

namespace SPELLCHECK
{

    //_______________________________________________
    const QString SpellInterface::NO_FILTER = "none";

    //_______________________________________________
    SpellInterface::SpellInterface( void ):
        Counter( "SpellInterface" ),
        text_(""),
        checkedText_(""),
        begin_(0),
        end_(0),
        position_(0),
        offset_(0),
        spellConfig_( new_aspell_config() ),
        spellChecker_( 0 ),
        documentChecker_( 0 )
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
        if( spellConfig_ ) delete_aspell_config( spellConfig_ );
        if( documentChecker_ ) delete_aspell_document_checker( documentChecker_ );
        if( spellChecker_ ) delete_aspell_speller( spellChecker_ );
        Debug::Throw( "SpellInterface::~SpellInterface - done.\n" );

    }

    //____________________________________________________
    bool SpellInterface::setDictionary( const QString& dictionary )
    {

        Debug::Throw( "SpellInterface::setDictionary.\n" );
        if( !spellConfig_ )
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
        if( spellChecker_ ) aspell_speller_save_all_word_lists( spellChecker_ );

        // update main dictionary
        aspell_config_replace(spellConfig_, "lang", dictionary.toAscii().constData() );
        dictionary_ = dictionary;

        // update personal dictionary
        QString personal_dictionary = Util::env( "HOME" ) + "/.aspell." + dictionary + ".pws";
        aspell_config_replace(spellConfig_, "personal", personal_dictionary.toAscii().constData() );

        // reset
        return _reset();

    }

    //____________________________________________________
    bool SpellInterface::setFilter( const QString& filter )
    {

        Debug::Throw( "SpellInterface::setFilter.\n" );
        if( !spellConfig_ ) {
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
        aspell_config_replace(spellConfig_, "mode", filter.toAscii().constData() );
        filter_ = filter;

        // reset SpellChecker
        return _reset() || filter == NO_FILTER;

    }

    //__________________________________________
    bool SpellInterface::setText(
        const QString& text,
        const int &begin,
        const int &end )
    {
        Debug::Throw( "SpellInterface::setText.\n" );

        // check text
        if( text.isEmpty() )
        {
            Debug::Throw( "SpellInterface::setText - empty.\n" );
            return true;
        }

        // store text
        text_ = text;
        checkedText_ = text;

        // store begin/end position
        begin_ = begin;
        end_ = end;

        // check limits
        assert( begin_ <= text_.size() && end_ <= text_.size() );

        end_ += offset_;
        position_ = 0;
        offset_ = 0;
        aspell_document_checker_reset( documentChecker_ );
        aspell_document_checker_process(documentChecker_, text_.mid( begin_, end_-begin_).toAscii().constData(), -1);

        return true;

    }


    //____________________________________________________
    bool SpellInterface::addWord( const QString& word )
    {
        Debug::Throw( "SpellInterface::addWord.\n" );

        // check word
        if( word.isEmpty() ) return false;

        // check spellchecker
        if( !spellChecker_ ) {
            error_ = "invalid aspell checker";
            return false;
        }

        // retrieve word from editor
        aspell_speller_add_to_personal(spellChecker_, word.toAscii().constData(), -1);
        return true;

    }

    //____________________________________________________
    bool SpellInterface::replace( const QString& word )
    {

        Debug::Throw( "SpellInterface::replace.\n" );

        // check current word
        if( !word_.size() )
        {
            error_ = "invalid word to be replaced";
            return false;
        }

        // check spell checker
        if( !spellChecker_ ) {
            error_ = "invalid aspell checker";
            return false;
        }


        // inform spellchecker of the replacement
        aspell_speller_store_replacement(
            spellChecker_,
            word_.toAscii().constData(), -1,
            word.toAscii().constData(), -1 );

        // update checked text
        checkedText_.replace( begin_+position_+offset_, word_.size(), word );
        offset_ += word.size() - word_.size();
        return true;

    }

    //____________________________________________________
    bool SpellInterface::nextWord( void )
    {

        // check filter
        if( filter_.isEmpty() )
        {
            error_ = "no filter set";
            return false;
        }

        // check filter
        if( dictionary_.isEmpty() )
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
        if( !documentChecker_ ) {
            error_ = "invalid aspell document checker";
            return false;
        }

        AspellToken token;
        while( token = aspell_document_checker_next_misspelling( documentChecker_ ), token.len )
        {

            // update position
            position_ = token.offset;

            // retrieve word
            word_ = text_.mid( begin_+position_, token.len );
            return true;
        }

        // spelling completed
        word_ = "";
        position_ = end_;

        return true;

    }

    //____________________________________________________
    QStringList SpellInterface::suggestions( const QString& word ) const
    {

        Debug::Throw( "SpellInterface::suggestions.\n" );

        // check spell checker
        QStringList out;
        if( !spellChecker_ ) {
            Debug::Throw(0, "SpellInterface::suggestions - no spell checker" );
            return out;
        }

        const AspellWordList * suggestions( aspell_speller_suggest( spellChecker_, word.toAscii().constData(), -1 ) );
        AspellStringEnumeration * elements( aspell_word_list_elements( suggestions ) );

        const char * suggestion( 0 );
        while( ( suggestion = aspell_string_enumeration_next( elements ) ) )
        { out << suggestion; }

        delete_aspell_string_enumeration( elements );
        return out;

    }

    //____________________________________________________
    void SpellInterface::saveWordList( void )
    {
        Debug::Throw( "SpellInterface::saveWordList.\n" );
        if( spellChecker_ ) aspell_speller_save_all_word_lists( spellChecker_ );
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
        if( !spellConfig_ ) return;

        const AspellDictInfoList * dictionaries( get_aspell_dict_info_list( spellConfig_ ) );
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

        QString command( XmlOptions::get().raw("ASPELL") + " dump modes" );
        FILE *tmp = popen( command.toAscii().constData(), "r" );
        static const int linesize( 128 );
        char buf[linesize];
        while( fgets( buf, linesize, tmp ) )
        {

            if( !strlen( buf ) ) continue;
            QTextStream in( buf );
            QString mode;
            in >> mode;
            if( in.status() == QTextStream::Ok && !mode.isEmpty() )
            { filters_.insert( mode ); }
        }

        pclose( tmp );
        return;

    }

    //____________________________________________________
    bool SpellInterface::_reset( void )
    {

        Debug::Throw( "SpellInterface::_reset\n" );

        // check config
        if( !spellConfig_ ) {
            error_ = "invalid aspell configuration";
            return false;
        }

        // delete document checker
        if( documentChecker_ ) delete_aspell_document_checker( documentChecker_ );
        documentChecker_ = 0;

        //!reset spell checker
        if( !_resetSpellChecker() )
        {
            Debug::Throw( "SpellInterface::_reset - _resetSpellChecker failed.\n" );
            return false;
        }

        // recreate document checker
        AspellCanHaveError* err = new_aspell_document_checker(spellChecker_);
        if( aspell_error_number(err) )
        {
            error_ = aspell_error_message( err );
            Debug::Throw( "SpellInterface::_reset - unable to create document_checker.\n" );
            return false;
        } else documentChecker_ = to_aspell_document_checker(err);

        // assign text if any
        if( checkedText_.size() )
        {
            Debug::Throw( "SpellInterface::_reset - assigning text.\n" );
            text_ = checkedText_;
            end_ += offset_;
            position_ = 0;
            offset_ = 0;
            aspell_document_checker_reset( documentChecker_ );
            aspell_document_checker_process(documentChecker_, text_.mid( begin_, end_-begin_).toAscii().constData(), -1);
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
        if( !spellConfig_ ) {
            Debug::Throw( "SpellInterface::_resetSpellChecker - invalid aspell configuration.\n" );
            error_ = "invalid aspell configuration";
            return false;
        }

        // delete spell checker
        if( spellChecker_ ) delete_aspell_speller( spellChecker_ );
        spellChecker_ = 0;

        // recreate spell checker
        AspellCanHaveError* err = new_aspell_speller(spellConfig_);
        if( aspell_error_number(err) )
        {
            Debug::Throw( "SpellInterface::_resetSpellChecker - failed to create speller.\n" );
            error_ = aspell_error_message( err );
            return false;
        } else spellChecker_ = to_aspell_speller(err);

        return true;

    }

}
