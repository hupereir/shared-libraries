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
   \file DictionaryMenu.cpp
   \brief dictionary dictionary selection menu
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "BaseIcons.h"
#include "DictionaryMenu.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "XmlOptions.h"

using namespace std;
using namespace SPELLCHECK;

//____________________________________________________________________
DictionaryMenu::DictionaryMenu( QWidget* parent ):
  QMenu( parent ),
  Counter( "DictionaryMenu" )
{ 
  Debug::Throw( "DictionaryMenu::DictionaryMenu.\n" );
  setTitle( "&Dictionary" );
  _reset();
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _selectDictionary( QAction* ) ) );
}

//____________________________________________________________________
void DictionaryMenu::select( const QString& dictionary )
{
  Debug::Throw( "DictionaryMenu::select.\n" );
  
  for( std::map<QAction*,QString>::iterator iter = action_map_.begin(); iter != action_map_.end(); iter++ )
  {
    QFont font( iter->first->font() );
    font.setWeight( (iter->second == dictionary ) ? QFont::Bold : QFont::Normal );
    iter->first->setFont( font );
  }  
   
  return;
  
}

//____________________________________________________________________
void DictionaryMenu::_reset( void )
{

  Debug::Throw( "DictionaryMenu::_reset.\n" );
  
  // clear actions
  QMenu::clear();
  action_map_.clear();

  // add reset button
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  addAction( "&Reload", this, SLOT( _reset() ) )->setIcon( IconEngine::get( ICONS::RELOAD, path_list ) );
  
  // load dictionaries from spell interface
  set< string > dictionaries( SPELLCHECK::SpellInterface().dictionaries() );
  if( !dictionaries.empty() ) addSeparator();
  
  for( set<string>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); iter++ )
  { 
    QAction* action( new QAction( iter->c_str(), this ) ); 
    action_map_.insert( make_pair( action, iter->c_str() ) );
    addAction( action );
  }
  
}

//______________________________________________________________________________________
void DictionaryMenu::_selectDictionary( QAction*  action )
{
  
  Debug::Throw( "DictionaryMenu::_dictionary.\n" );
  std::map<QAction*,QString>::iterator iter( action_map_.find( action ) );
  if( iter == action_map_.end() ) return;
  
  select( iter->second );
  emit selectionChanged( iter->second );
  return;
  
}
