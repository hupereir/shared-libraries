#ifndef _SuggestionMenu_h_
#define _SuggestionMenu_h_

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

/*!
   \file SuggestionMenu.h
   \brief customized popup menu to display spell checker suggestions
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QMenu>

#include <map>
#include <string>

#include "Counter.h"
#include "SpellInterface.h"

/*!
   \class SuggestionMenu
   \brief customized popup menu to display spell checker suggestions
*/

namespace SPELLCHECK {
  
  class SuggestionMenu: public QMenu, public Counter
  {
  
    //! Qt meta object declaration
    Q_OBJECT
    
    public:
    
    //! constructor
    SuggestionMenu( QWidget *parent, const std::string& word, const bool& read_only = false );
  
    //! spell interface
    SpellInterface& interface( void )
    { return interface_; }
    
    signals:
  
    //! signal emited when a file is selected  
    void suggestionSelected( std::string );
   
    //! ignore word
    void ignoreWord( std::string );
        
    private slots:
    
    //! load suggestions before showing the menu
    void _aboutToShow( void );
    
    //! select word from action
    void _select( QAction* );
    
    //! add word to dictionary
    void _addWord( void );
    
    //! ignore word
    void _ignoreWord( void )
    { 
      if( word_.empty() ) return;
      emit ignoreWord( word_ ); 
    }
  
    private:
    
    //! spell interface
    SpellInterface interface_;
        
    //! misspelled word
    std::string word_;

    //! map actions to suggested words
    std::map<QAction*, std::string> suggestions_;
    
  };
}
  
#endif
