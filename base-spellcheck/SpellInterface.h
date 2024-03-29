#ifndef SpellInterface_h
#define SpellInterface_h

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
#include "base_spellcheck_export.h"

#include <QSet>
#include <QStringList>

#include <aspell.h>
#include <memory>

namespace SpellCheck
{
    //* spell checker interface
    class BASE_SPELLCHECK_EXPORT SpellInterface final: private Base::Counter<SpellInterface>
    {

        public:

        //* constructor
        explicit SpellInterface();

        //* destructor
        ~SpellInterface();

        //* no filter
        static const QString FilterNone;

        //* latex
        static const QString FilterTex;

        //* latex no accents
        static const QString FilterTexWithNoAccents;

        //*@name accessors
        //@{

        //* get list of available dictionaries
        using DictionarySet = QOrderedSet<QString>;
        const DictionarySet& dictionaries() const
        { return dictionaries_; }

        //* list dictionaries
        void listDictionaries() const;

        //* current dictionary
        const QString dictionary() const
        { return dictionary_; }

        //* true if dictionary is available
        bool hasDictionary( const QString& dictionary ) const
        { return dictionaries_.find( dictionary ) != dictionaries_.end(); }

        //* get list of available filters
        using FilterSet = QOrderedSet<QString>;
        const FilterSet& filters() const
        { return filters_; }

        //* list dictionaries
        void listFilters() const;

        //* current filter
        const QString filter() const
        { return filter_; }

        //* true if filter is available
        bool hasFilter( const QString& filter ) const
        { return filters_.find( filter ) != filters_.end(); }

        //* returns true if word is ignored
        bool isWordIgnored( const QString& word )
        { return ignoredWords_.find( word ) != ignoredWords_.end(); }

        //* get list of ignored words
        const QSet< QString > ignoredWords()
        { return ignoredWords_; }

        //* retrieve original text
        const QString& text() const
        { return text_; }

        //* retrieve checked text
        const QString& checkedText() const
        { return checkedText_; }

        //* retrieve error
        const QString& error() const
        { return error_; }

        //* retrieve current word being checked
        const QString& word() const
        { return word_; }

        //* retrieve position of current word in original text
        int position() const
        { return position_+begin_; }

        //* retrieve position offset between original text and checked text
        int offset() const
        { return offset_; }

        //* retrieve list of suggestions for current word
        QStringList suggestions( const QString& ) const;

        //@}

        //*@name modifiers
        //@{

        //* select dictionary
        /** returns false in case of error */
        bool setDictionary( const QString& );

        //* select filter
        /** returns false in case of error */
        bool setFilter( const QString& );

        //* clear filters
        void clearFilters()
        {
            filters_.clear();
            filterMap_.clear();
        }

        //* add word to static list
        void ignoreWord( const QString& word )
        { ignoredWords_.insert( word ); }

        //* set list of ignored words
        void setIgnoredWords( const QSet<QString>& words )
        { ignoredWords_ = words; }

        //* merge list of ignored words
        void mergeIgnoredWords( const QSet<QString>& words )
        { ignoredWords_.unite( words ); }

        //* clear ignored words
        void clearIgnoredWords()
        { ignoredWords_.clear(); }

        //* set text to be checked
        bool setText( const QString& text )
        { return setText( text, 0, text.size() ); }

        //* set text to be checked
        /**
        returns false on error
        \param text the text in which selection is to be checked
        \param begin selection begin position
        \param end selection end position in the text
        */
        bool setText( const QString& text, int begin, int end );

        //* add word to dictionary
        /** return false on error */
        bool addWord( const QString& word );

        //* add current word to dictionary
        /** return false on error */
        bool addWord()
        { return addWord( word() ); }

        //* replace current word with argument in checked text
        bool replace( const QString& new_word );

        //* retrieve next word to be checked
        /** return false when there is an error */
        bool nextWord();

        //* save word list in personal dictionary
        void saveWordList();

        //* restart checking
        /**
        initial begin/end are reset to the full text
        return false on error
        */
        bool reset();

        //@}

        private:

        //* load available dictionaries
        void _loadDictionaries();

        //* load available filters
        void _loadFilters();

        //* reset Aspell spell/document checker
        /** returns false on error */
        bool _reset();

        //* reset Aspell spell checker
        /** returns false on error */
        bool _resetSpellChecker();

        //* set of available dictionaries
        /** loaded in constructor */
        DictionarySet dictionaries_;

        //* set of available filters
        using FilterMap = QHash<QString, QString>;
        static FilterMap filterMap_;

        //* set of available filters
        /** loaded in constructor */
        static FilterSet filters_;

        //* ignored words
        QSet<QString> ignoredWords_;

        //* original text to be checked
        QString text_;

        //* text being checked
        QString checkedText_;

        //* current word being checked
        QString word_;

        //* begin position to be checked in text
        int begin_ = 0;

        //* end position to be checked in text
        int end_ = 0;

        //* position in started text (counted from begin_)
        int position_ = 0;

        //* offset between checkedText_ and text_
        int offset_ = 0;

        //* configuration error
        QString error_;

        //* dictionary
        QString dictionary_;

        //* filter
        QString filter_;

        //* Aspell config deleter
        class BASE_SPELLCHECK_EXPORT AspellConfigDeleter
        {

            public:

            //* deleter
            void operator()(AspellConfig* value) const
            { delete_aspell_config( value ); }

        };

        //* aspell configuration singleton
        using AspellConfigPtr = std::unique_ptr<AspellConfig, AspellConfigDeleter>;
        AspellConfigPtr spellConfig_;

        //* Aspell checker deleter
        class BASE_SPELLCHECK_EXPORT AspellSpellerDeleter
        {

            public:

            //* deleter
            void operator()(AspellSpeller* value) const
            { delete_aspell_speller( value ); }

        };

        //* aspell checker
        using AspellSpellerPtr = std::unique_ptr<AspellSpeller, AspellSpellerDeleter>;
        AspellSpellerPtr spellChecker_;

        //* Aspell document checker deleter
        class BASE_SPELLCHECK_EXPORT AspellDocumentCheckerDeleter
        {

            public:

            //* deleter
            void operator()(AspellDocumentChecker* value) const
            { delete_aspell_document_checker( value ); }

        };


        using AspellDocumentCheckerPtr = std::unique_ptr<AspellDocumentChecker, AspellDocumentCheckerDeleter>;
        AspellDocumentCheckerPtr documentChecker_;
    };

}

#endif
