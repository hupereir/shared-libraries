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

#include "SuggestionMenu.h"

#include "Debug.h"
#include "TexString.h"
#include "XmlOptions.h"

namespace SpellCheck
{

    //________________________________________________
    SuggestionMenu::SuggestionMenu( QWidget* parent, const QString& word, bool readOnly ):
        QMenu( parent ),
        Counter( "SuggestionMenu" ),
        word_( word )
    {

        Debug::Throw( "SuggestionMenu::SuggestionMenu.\n" );

        // item selection
        interface_.reset();

        // connections
        if( !readOnly ) connect( this, SIGNAL(triggered(QAction*)), SLOT(_select(QAction*)) );
        connect( this, SIGNAL(aboutToShow()), SLOT(_aboutToShow()) );

    }

    //________________________________________________________
    void SuggestionMenu::_aboutToShow( void )
    {

        Debug::Throw( "SuggestionMenu::_aboutToShow.\n" );

        QString word( word_ );

        // convert accents
        if( interface().filter() == SpellInterface::FilterTexWithNoAccents ) word = TexString( word ).toTextAccents();

        // retrieve list of suggestions
        QStringList suggestions( interface_.suggestions( word ) );
        Debug::Throw() << "SuggestionMenu::_aboutToShow - suggestions: " << suggestions.size() << endl;

        // add words
        int max( XmlOptions::get().get<int>( "MAX_SUGGESTIONS" ) );
        if( max == 0 || max > suggestions.size() ) max = suggestions.size();
        for( int i=0; i < max; ++i )
        {

            QString suggestion( suggestions[i] );
            if( interface().filter() == SpellInterface::FilterTexWithNoAccents ) suggestion = TexString( suggestion ).toLatexAccents();

            Debug::Throw() << "SuggestionMenu::_aboutToShow - adding: " << suggestion << endl;
            suggestions_.insert( addAction( suggestions[i] ), suggestion );
        }

        // separator
        if( !suggestions.isEmpty() )
        { addSeparator(); }

        // add word action
        addAction( tr( "Add Word to Dictionary" ), this, SLOT(_addWord()) );

        // ignore word action
        addAction( tr( "Ignore Word" ), this, SLOT(_ignoreWord()) );

    }

    //________________________________________________
    void SuggestionMenu::_select( QAction* action )
    {

        Debug::Throw( "SuggestionMenu::_select.\n" );
        SuggestionMap::const_iterator iter( suggestions_.find( action ) );
        if( iter != suggestions_.end() ) emit suggestionSelected( iter.value() );
        return;

    }

    //________________________________________________
    void SuggestionMenu::_addWord( void )
    {

        Debug::Throw( "SuggestionMenu::_addWord.\n" );
        if( word_.isEmpty() ) return;
        interface_.addWord( word_ );
        interface_.saveWordList();
        emit ignoreWord( word_ );

    }

}