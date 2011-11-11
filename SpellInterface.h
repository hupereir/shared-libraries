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

#include "Counter.h"

#include <aspell.h>
#include <vector>
#include <set>

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
        static const QString NO_FILTER;

        //!@name dictionary
        //@{

        //! get list of available dictionaries
        const std::set< QString >& dictionaries( void ) const
        { return dictionaries_; }

        //! current dictionary
        const QString dictionary( void ) const
        { return dictionary_; }

        //! true if dictionary is available
        bool hasDictionary( const QString& dictionary ) const
        { return dictionaries_.find( dictionary ) != dictionaries_.end(); }

        //! select dictionary
        /*! returns false in case of error */
        bool setDictionary( const QString& dictionary );

        //@}

        //!@name filter
        //@{

        //! get list of available filters
        const std::set< QString >& filters( void ) const
        { return filters_; }

        //! current filter
        const QString filter( void ) const
        { return filter_; }

        //! true if filter is available
        bool hasFilter( const QString& filter ) const
        { return filters_.find( filter ) != filters_.end(); }

        //! select filter
        /*! returns false in case of error */
        bool setFilter( const QString& filter );

        //@}

        //!@name ignored words
        //@{

        //! add word to static list
        void ignoreWord( const QString& word )
        { ignored_words_.insert( word ); }

        //! returns true if word is ignored
        bool isWordIgnored( const QString& word )
        { return ignored_words_.find( word ) != ignored_words_.end(); }

        //! set list of ignored words
        void setIgnoredWords( const std::set<QString> words )
        { ignored_words_ = words; }

        //! merge list of ignored words
        void mergeIgnoredWords( const std::set<QString> words )
        { ignored_words_.insert( words.begin(), words.end() ); }

        //! clear ignored words
        void clearIgnoredWords( void )
        { ignored_words_.clear(); }

        //! get list of ignored words
        const std::set< QString > ignoredWords( void )
        { return ignored_words_; }

        //!@name text
        //@{

        //! set text to be checked
        bool setText( const QString& text )
        { return setText( text, 0, text.size() ); }

        //! set text to be checked
        /*!
        returns false on error
        \param text the text in which selection is to be checked
        \param begin selection begin position
        \param end selection end position in the text
        */
        bool setText( const QString& text, const int& begin, const int& end );

        //! retrieve original text
        const QString& text( void ) const
        { return text_; }

        //! retrieve checked text
        const QString& checkedText( void ) const
        { return checked_text_; }

        //@}

        //! retrieve error
        const QString& error( void ) const
        { return error_; }


        //! retrieve current word being checked
        const QString& word( void ) const
        { return word_; }

        //! retrieve position of current word in original text
        int position( void ) const
        { return position_+begin_; }

        //! retrieve position offset between original text and checked text
        const int& offset( void ) const
        { return offset_; }

        //! add word to dictionary
        /*! return false on error */
        bool addWord( const QString& word );

        //! add current word to dictionary
        /*! return false on error */
        bool addWord( void )
        { return addWord( word() ); }

        //! replace current word with argument in checked text
        bool replace( const QString& new_word );

        //! retrieve next word to be checked
        /*! return false when there is an error */
        bool nextWord( void );

        //! retrieve list of suggestions for current word
        std::vector< QString > suggestions( const QString& word ) const;

        //! save word list in personal dictionary
        void saveWordList( void );

        //! restart checking
        /*!
        initial begin/end are reset to the full text
        return false on error
        */
        bool reset( void );

        private:

        //! load available dictionaries
        void _loadDictionaries( void );

        //! load available filters
        void _loadFilters( void );

        //! reset Aspell spell/document checker
        /*! returns false on error */
        virtual bool _reset( void );

        //! reset Aspell spell checker
        /*! returns false on error */
        virtual bool _resetSpellChecker( void );

        //! set of available dictionaries
        /*! loaded in constructor */
        std::set<QString> dictionaries_;

        //! set of available filters
        /*! loaded in constructor */
        std::set<QString> filters_;

        //! ignored words
        std::set<QString> ignored_words_;

        //! original text to be checked
        QString text_;

        //! text being checked
        QString checked_text_;

        //! current word being checked
        QString word_;

        //! begin position to be checked in text
        int begin_;

        //! end position to be checked in text
        int end_;

        //! position in started text (counted from begin_)
        int position_;

        //! offset between checked_text_ and text_
        int offset_;

        //! configuration error
        QString error_;

        //! dictionary
        QString dictionary_;

        //! filter
        QString filter_;

        //! aspell configuration singleton
        AspellConfig* spellConfig_;

        //! aspell checker
        AspellSpeller* spellChecker_;

        //! aspell checker
        AspellDocumentChecker* documentChecker_;
    };

}

#endif
