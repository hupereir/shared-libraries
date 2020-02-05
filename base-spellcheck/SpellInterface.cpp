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

#include "CustomProcess.h"
#include "Debug.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QRegularExpression>

#include <algorithm>
#include <functional>

namespace SpellCheck
{

    SpellInterface::FilterMap SpellInterface::filterMap_;
    SpellInterface::FilterSet SpellInterface::filters_;

    //! used to select tag of maximum length
    class MinLengthFTor
    {

        public:

        //! predicate
        bool operator () ( const QString& first, const QString& second )
        { return first.size() < second.size(); }

    };

    //_______________________________________________
    const QString SpellInterface::FilterNone = QStringLiteral( "none" );
    const QString SpellInterface::FilterTex = QStringLiteral( "tex" );
    const QString SpellInterface::FilterTexWithNoAccents = QObject::tr( "tex (no accents)" );

    //_______________________________________________
    SpellInterface::SpellInterface():
        Counter( QStringLiteral("SpellInterface") ),
        spellConfig_( new_aspell_config() )
    {
        Debug::Throw( QStringLiteral("SpellInterface::SpellInterface.\n") );

        // set encoding. Forced to latin1
        aspell_config_replace(spellConfig_.get(), "encoding", "iso8859-1" );

        // load dictionaries and filters
        _loadDictionaries();
        _loadFilters();
        _resetSpellChecker();

        // set default dictionary
        if( !dictionaries().empty() ) setDictionary( *dictionaries().begin() );

        // set default filter
        setFilter( FilterNone );

        Debug::Throw( QStringLiteral("SpellInterface::SpellInterface - done.\n") );

    }

    //__________________________________________
    SpellInterface::~SpellInterface()
    { saveWordList(); }

    //____________________________________________________
    void SpellInterface::listDictionaries() const
    {
        if( dictionaries_.isEmpty() ) return;
        QTextStream(stdout) << QObject::tr( "Available dictionaries: " ) << endl;
        for( const auto& dictionary:dictionaries_ )
        { QTextStream(stdout) << "  " << dictionary << endl; }
    }

    //____________________________________________________
    void SpellInterface::listFilters() const
    {
        if( dictionaries_.isEmpty() ) return;
        QTextStream(stdout) << QObject::tr( "Available filters: " ) << endl;

        // maximum length
        const int maxLength = std::max_element( filters_.constBegin(), filters_.constEnd(), MinLengthFTor() )->size();
        for( auto iter = filterMap_.constBegin(); iter != filterMap_.constEnd(); ++iter )
        { QTextStream(stdout) << "  " << iter.key().leftJustified( maxLength + 1 ) <<  iter.value() << endl; }

    }

    //____________________________________________________
    bool SpellInterface::setDictionary( const QString& dictionary )
    {

        Debug::Throw( QStringLiteral("SpellInterface::setDictionary.\n") );
        if( !spellConfig_ )
        {
            error_ = QObject::tr( "invalid aspell configuration" );
            return false;
        }

        // check dictionary
        if( !hasDictionary( dictionary ) )
        {
            error_ = QObject::tr( "invalid dictionary" );
            return false;
        }

        // save previous personal dictionary
        if( spellChecker_ ) aspell_speller_save_all_word_lists( spellChecker_.get() );

        // update main dictionary
        aspell_config_replace(spellConfig_.get(), "lang", dictionary.toLatin1().constData() );
        dictionary_ = dictionary;

        // update personal dictionary
        QString personal_dictionary = Util::env( "HOME" ) + "/.aspell." + dictionary + ".pws";
        aspell_config_replace(spellConfig_.get(), "personal", personal_dictionary.toLatin1().constData() );

        // reset
        return _reset();

    }

    //____________________________________________________
    bool SpellInterface::setFilter( const QString& filter )
    {

        Debug::Throw( QStringLiteral("SpellInterface::setFilter.\n") );
        if( !spellConfig_ )
        {
            error_ = QObject::tr( "invalid aspell configuration" );
            return false;
        }

        // check dictionary
        if( !hasFilter( filter ) )
        {
            error_ = QObject::tr( "invalid filter" );
            return false;
        }

        // update filter
        filter_ = filter;

        // update aspell
        aspell_config_replace(spellConfig_.get(), "mode", (filter == FilterTexWithNoAccents ? FilterTex:filter).toLatin1().constData() );

        // reset SpellChecker
        return _reset() || filter == FilterNone;

    }

    //__________________________________________
    bool SpellInterface::setText(
        const QString& text,
        int begin,
        int end )
    {
        Debug::Throw( QStringLiteral("SpellInterface::setText.\n") );

        // check text
        if( text.isEmpty() )
        {
            Debug::Throw( QStringLiteral("SpellInterface::setText - empty.\n") );
            return true;
        }

        // store text
        text_ = text;
        checkedText_ = text;

        // store begin/end position
        begin_ = begin;
        end_ = end;

        // check limits
        Q_ASSERT( begin_ <= text_.size() && end_ <= text_.size() );

        end_ += offset_;
        position_ = 0;
        offset_ = 0;
        aspell_document_checker_reset( documentChecker_.get() );
        aspell_document_checker_process(documentChecker_.get(), text_.mid( begin_, end_-begin_).toLatin1().constData(), -1);

        return true;

    }


    //____________________________________________________
    bool SpellInterface::addWord( const QString& word )
    {
        Debug::Throw( QStringLiteral("SpellInterface::addWord.\n") );

        // check word
        if( word.isEmpty() ) return false;

        // check spellchecker
        if( !spellChecker_ ) {
            error_ = QObject::tr( "invalid aspell checker" );
            return false;
        }

        // retrieve word from editor
        aspell_speller_add_to_personal(spellChecker_.get(), word.toLatin1().constData(), -1);
        return true;

    }

    //____________________________________________________
    bool SpellInterface::replace( const QString& word )
    {

        Debug::Throw( QStringLiteral("SpellInterface::replace.\n") );

        // check current word
        if( !word_.size() )
        {
            error_ = QObject::tr( "invalid word to be replaced" );
            return false;
        }

        // check spell checker
        if( !spellChecker_ )
        {
            error_ = QObject::tr( "invalid aspell checker" );
            return false;
        }


        // inform spellchecker of the replacement
        aspell_speller_store_replacement(
            spellChecker_.get(),
            word_.toLatin1().constData(), -1,
            word.toLatin1().constData(), -1 );

        // update checked text
        checkedText_.replace( begin_+position_+offset_, word_.size(), word );
        offset_ += word.size() - word_.size();
        return true;

    }

    //____________________________________________________
    bool SpellInterface::nextWord()
    {

        // check filter
        if( filter_.isEmpty() )
        {
            error_ = QObject::tr( "no filter set" );
            return false;
        }

        // check filter
        if( dictionary_.isEmpty() )
        {
            error_ = QObject::tr( "no dictionary set" );
            return false;
        }

        // check text size
        if( !text_.size() )
        {
            word_ = "";
            error_ = QObject::tr( "no text to check" );
            return false;
        }

        // check document checker
        if( !documentChecker_ ) {
            error_ = QObject::tr( "invalid aspell document checker" );
            return false;
        }

        AspellToken token;
        while( token = aspell_document_checker_next_misspelling( documentChecker_.get() ), token.len )
        {

            // update position
            position_ = token.offset;

            // retrieve word
            word_ = QString::fromLatin1( text_.toLatin1().mid( begin_+position_, token.len ) );

            return true;
        }

        // spelling completed
        word_.clear();
        position_ = end_;

        return true;

    }

    //____________________________________________________
    QStringList SpellInterface::suggestions( const QString& word ) const
    {

        Debug::Throw( QStringLiteral("SpellInterface::suggestions.\n") );

        // check spell checker
        QStringList out;
        if( !spellChecker_ ) {
            Debug::Throw(0, "SpellInterface::suggestions - no spell checker" );
            return out;
        }

        const auto suggestions( aspell_speller_suggest( spellChecker_.get(), word.toLatin1().constData(), -1 ) );

        // get corresponding word list
        std::unique_ptr<AspellStringEnumeration, std::function<void(AspellStringEnumeration*)>> elements(
            aspell_word_list_elements( suggestions ),
            [](AspellStringEnumeration* elements){ delete_aspell_string_enumeration( elements ); } );

        const char* suggestion( nullptr );
        while( ( suggestion = aspell_string_enumeration_next( elements.get() ) ) )
        { out.append( QString::fromLatin1( suggestion ) ); }

        return out;

    }

    //____________________________________________________
    void SpellInterface::saveWordList()
    {
        Debug::Throw( QStringLiteral("SpellInterface::saveWordList.\n") );
        if( spellChecker_ ) aspell_speller_save_all_word_lists( spellChecker_.get() );
    }


    //____________________________________________________
    bool SpellInterface::reset()
    {
        Debug::Throw( QStringLiteral("SpellInterface::reset.\n") );

        // reinitialize begin and end position
        begin_ = 0;
        end_ = text_.size();

        // reset
        return _reset();

    }


    //____________________________________________________
    void SpellInterface::_loadDictionaries()
    {

        Debug::Throw( QStringLiteral("SpellInterface::_loadDictionaries.\n") );

        dictionaries_.clear();
        if( !spellConfig_ ) return;

        // todo: encapsulate
        const auto dictionaries( get_aspell_dict_info_list( spellConfig_.get() ) );
        std::unique_ptr<AspellDictInfoEnumeration,std::function<void(AspellDictInfoEnumeration*)>> elements(
            aspell_dict_info_list_elements( dictionaries ),
            []( AspellDictInfoEnumeration* elements ){ delete_aspell_dict_info_enumeration( elements ); } );

        const AspellDictInfo* dictionary( nullptr );
        while( ( dictionary = aspell_dict_info_enumeration_next( elements.get() ) ) )
        { if( dictionary->code ) dictionaries_.insert( dictionary->code ); }

        return;

    }

    //____________________________________________________
    void SpellInterface::_loadFilters()
    {
        if( !filters_.isEmpty() ) return;

        Debug::Throw() << "SpellInterface::_loadFilters- this: " << this << endl;

        // TODO: use QProcess here instead of popen
        filters_.insert( FilterNone );

        CustomProcess process;
        process.start( XmlOptions::get().raw("ASPELL") + " dump modes" );
        process.waitForFinished();
        const auto lines( QString( process.readAllStandardOutput() ).split( QLatin1Char('\n') ) );
        for( const auto& line:lines )
        {
            static const QRegularExpression regExp( QStringLiteral("^(\\w+)\\s+(.*)") );
            const auto match( regExp.match( line ) );
            if( match.hasMatch() )
            {
                const auto mode( match.captured(1) );
                const QString comment( match.captured(2) );
                filters_.insert( mode );
                filterMap_.insert( mode, comment );
                if( mode == FilterTex )
                {
                    filters_.insert( FilterTexWithNoAccents );
                    filterMap_.insert( FilterTexWithNoAccents, comment );
                }
            }

        }

        return;

    }

    //____________________________________________________
    bool SpellInterface::_reset()
    {

        Debug::Throw( QStringLiteral("SpellInterface::_reset\n") );

        // check config
        if( !spellConfig_ ) {
            error_ = QObject::tr( "invalid aspell configuration" );
            return false;
        }

        // delete document checker
        documentChecker_.reset();

        //!reset spell checker
        if( !_resetSpellChecker() )
        {
            Debug::Throw( QStringLiteral("SpellInterface::_reset - _resetSpellChecker failed.\n") );
            return false;
        }

        // recreate document checker
        AspellCanHaveError* err = new_aspell_document_checker(spellChecker_.get());
        if( aspell_error_number(err) )
        {

            error_ = aspell_error_message( err );
            Debug::Throw( QStringLiteral("SpellInterface::_reset - unable to create document_checker.\n") );
            return false;

        } else documentChecker_.reset( to_aspell_document_checker(err) );

        // assign text if any
        if( checkedText_.size() )
        {
            Debug::Throw( QStringLiteral("SpellInterface::_reset - assigning text.\n") );
            text_ = checkedText_;
            end_ += offset_;
            position_ = 0;
            offset_ = 0;
            aspell_document_checker_reset( documentChecker_.get() );
            aspell_document_checker_process(documentChecker_.get(), text_.mid( begin_, end_-begin_).toLatin1().constData(), -1);
            Debug::Throw( QStringLiteral("SpellInterface::_reset - assigning text, done.\n") );
        }

        return true;

    }

    //____________________________________________________
    bool SpellInterface::_resetSpellChecker()
    {

        Debug::Throw( QStringLiteral("SpellInterface::_resetSpellChecker\n") );

        // check config
        if( !spellConfig_ )
        {
            Debug::Throw( QStringLiteral("SpellInterface::_resetSpellChecker - invalid aspell configuration.\n") );
            error_ = QObject::tr( "invalid aspell configuration" );
            return false;
        }

        // delete spell checker
        spellChecker_.reset();

        // recreate spell checker
        AspellCanHaveError* err = new_aspell_speller(spellConfig_.get());
        if( aspell_error_number(err) )
        {

            Debug::Throw( QStringLiteral("SpellInterface::_resetSpellChecker - failed to create speller.\n") );
            error_ = aspell_error_message( err );
            return false;

        } else spellChecker_.reset( to_aspell_speller(err) );

        return true;

    }

}
