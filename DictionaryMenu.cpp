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

using namespace std;
using namespace SPELLCHECK;

//____________________________________________________________________
DictionaryMenu::DictionaryMenu( QWidget* parent ):
  QMenu( parent ),
  Counter( "DictionaryMenu" )
{ 
  
  Debug::Throw( "DictionaryMenu::DictionaryMenu.\n" );
  setTitle( "&Dictionary" );
  
  // action group
  group_ = new QActionGroup( this );
  group_->setExclusive( true );
  
  _reset();
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _selectDictionary( QAction* ) ) );

}

//____________________________________________________________________
void DictionaryMenu::select( const QString& dictionary )
{
  Debug::Throw( "DictionaryMenu::select.\n" );
  
  for( std::map<QAction*,QString>::iterator iter = action_map_.begin(); iter != action_map_.end(); iter++ )
  { if( iter->second == dictionary ) iter->first->setChecked( true ); }  
   
  return;
  
}

//____________________________________________________________________
void DictionaryMenu::_reset( void )
{

  Debug::Throw( "DictionaryMenu::_reset.\n" );
  
  // store selected dictionary
  QString dictionary;
  for( std::map<QAction*,QString>::iterator iter = action_map_.begin(); iter != action_map_.end(); iter++ )
  { if( iter->first->isChecked() ) dictionary = iter->second; }
  
  // clear actions
  QMenu::clear();
  action_map_.clear();

  // add reset button
  QAction* action;
  addAction( action = new QAction( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
  action->setShortcut( Qt::Key_F5 );
  connect( action, SIGNAL( triggered() ), SLOT( _reset() ) );
  
  // load dictionaries from spell interface
  set< string > dictionaries( SPELLCHECK::SpellInterface().dictionaries() );
  if( !dictionaries.empty() ) addSeparator();
  
  for( set<string>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); iter++ )
  { 
    QAction* action( new QAction( iter->c_str(), this ) ); 
    action->setCheckable( true );
    action->setChecked( iter->c_str() == dictionary );
    action_map_.insert( make_pair( action, iter->c_str() ) );
    addAction( action );
    group_->addAction( action );
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
