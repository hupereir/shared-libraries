#ifndef SpellInterface_h
#define SpellInterface_h

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

#include "Counter.h"
#include "QOrderedSet.h"

#include <QSet>
#include <QStringList>

#include <aspell.h>

namespace SpellCheck
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

        //! latex
        static const QString FILTER_TEX;

        //! latex no accents
        static const QString FILTER_TEX_NO_ACCENTS;

        //!@name accessors
        //@{

        //! get list of available dictionaries
        typedef QOrderedSet<QString> DictionarySet;
        const DictionarySet& dictionaries( void ) const
        { return dictionaries_; }

        //! list dictionaries
        void listDictionaries( void ) const;

        //! current dictionary
        const QString dictionary( void ) const
        { return dictionary_; }

        //! true if dictionary is available
        bool hasDictionary( const QString& dictionary ) const
        { return dictionaries_.find( dictionary ) != dictionaries_.end(); }

        //! get list of available filters
        typedef QOrderedSet<QString> FilterSet;
        const FilterSet& filters( void ) const
        { return filters_; }

        //! list dictionaries
        void listFilters( void ) const;

        //! current filter
        const QString filter( void ) const
        { return filter_; }

        //! true if filter is available
        bool hasFilter( const QString& filter ) const
        { return filters_.find( filter ) != filters_.end(); }

        //! returns true if word is ignored
        bool isWordIgnored( const QString& word )
        { return ignoredWords_.find( word ) != ignoredWords_.end(); }

        //! get list of ignored words
        const QSet< QString > ignoredWords( void )
        { return ignoredWords_; }

        //! retrieve original text
        const QString& text( void ) const
        { return text_; }

        //! retrieve checked text
        const QString& checkedText( void ) const
        { return checkedText_; }

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
        int offset( void ) const
        { return offset_; }

        //! retrieve list of suggestions for current word
        QStringList suggestions( const QString& word ) const;

        //@}

        //!@name modifiers
        //@{

        //! select dictionary
        /*! returns false in case of error */
        bool setDictionary( const QString& dictionary );

        //! select filter
        /*! returns false in case of error */
        bool setFilter( const QString& filter );

        //! clear filters
        void clearFilters( void )
        { filters_.clear(); }

        //! add word to static list
        void ignoreWord( const QString& word )
        { ignoredWords_.insert( word ); }

        //! set list of ignored words
        void setIgnoredWords( const QSet<QString> words )
        { ignoredWords_ = words; }

        //! merge list of ignored words
        void mergeIgnoredWords( const QSet<QString> words )
        { ignoredWords_.unite( words ); }

        //! clear ignored words
        void clearIgnoredWords( void )
        { ignoredWords_.clear(); }

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
        bool setText( const QString& text, int begin, int end );

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

        //! save word list in personal dictionary
        void saveWordList( void );

        //! restart checking
        /*!
        initial begin/end are reset to the full text
        return false on error
        */
        bool reset( void );

        //@}

        protected:

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

        private:

        //! set of available dictionaries
        /*! loaded in constructor */
        DictionarySet dictionaries_;

        //! set of available filters
        /*! loaded in constructor */
        static FilterSet filters_;

        //! ignored words
        QSet<QString> ignoredWords_;

        //! original text to be checked
        QString text_;

        //! text being checked
        QString checkedText_;

        //! current word being checked
        QString word_;

        //! begin position to be checked in text
        int begin_;

        //! end position to be checked in text
        int end_;

        //! position in started text (counted from begin_)
        int position_;

        //! offset between checkedText_ and text_
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
