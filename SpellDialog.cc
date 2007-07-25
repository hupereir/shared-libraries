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
  \file SpellDialog.cc
  \brief spell checker popup dialog
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLayout>
#include <QPushButton>
#include <QTextCursor>

#include "Debug.h"
#include "QtUtil.h"
#include "SpellDialog.h"
#include "SpellInterface.h"
#include "Util.h"
#include "XmlOptions.h"

using namespace std;
using namespace SPELLCHECK;

//_______________________________________________
SpellDialog::SpellDialog( QTextEdit* parent ):
  QDialog( parent ),
  Counter( "SpellDialog" ),
  editor_( parent )
{
  Debug::Throw( "SpellDialog::SpellDialog.\n" );

  // create vbox layout
  QVBoxLayout* layout=new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(2);
  setLayout( layout );
  
  // horizontal layout for suggestions and buttons
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setMargin(0);
  h_layout->setSpacing(2);
  layout->addLayout( h_layout, 1 );
  
  // insert left vertical box
  QVBoxLayout *v_layout = new QVBoxLayout();
  v_layout->setMargin( 0 );
  v_layout->setSpacing( 2 );
  h_layout->addLayout( v_layout );
  
  // grid for text editors
  QGridLayout *grid_layout = new QGridLayout();
  grid_layout->setMargin( 0 );
  grid_layout->setSpacing( 2 );
  v_layout->addLayout( grid_layout );
  
  // misspelled word line editor
  grid_layout->addWidget( new QLabel( "Misspelled word: ", this ), 0, 0 );
  grid_layout->addWidget( line_edit_ = new CustomLineEdit( this ), 0, 1 );
  line_edit_->setReadOnly( true );

  // replacement line editor
  grid_layout->addWidget( new QLabel( "Replace with: ", this ), 1, 0 );
  grid_layout->addWidget( replace_line_edit_ = new CustomLineEdit( this ), 1, 1 );
  
  QLabel* label = new QLabel( "Suggestions: ", this );
  QtUtil::fixSize( label );
  v_layout->addWidget( label );

  // suggestions
  v_layout->addWidget(  suggestion_list_box_ = new CustomListBox( this ), 1 );
  connect( suggestion_list_box_, SIGNAL( itemSelectionChanged() ), this, SLOT( _selectSuggestion() ) );
  connect( suggestion_list_box_, SIGNAL( itemActivated( QListWidgetItem* ) ), this, SLOT( _replace( QListWidgetItem* ) ) );

  // grid layout for dictionary and filter
  grid_layout = new QGridLayout();
  grid_layout->setMargin( 0 );
  grid_layout->setSpacing( 2 );
  v_layout->addLayout( grid_layout );
  
  // dictionaries combobox
  grid_layout->addWidget( new QLabel( "Dictionary: ", this ), 0, 0 );
  grid_layout->addWidget( dict_combo_box_ = new QComboBox( this ), 0, 1 );

  // filter combobox
  grid_layout->addWidget( filter_label_ = new QLabel( "Filter: ", this ), 1, 0 );
  grid_layout->addWidget( filter_combo_box_ = new QComboBox( this ), 1, 1 );

  // right vbox
  v_layout = new QVBoxLayout();
  v_layout->setMargin( 0 );
  v_layout->setSpacing( 2 );
  h_layout->addLayout( v_layout );

  // add word button
  QPushButton* button;
  v_layout->addWidget( button = new QPushButton( "&Add word", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _addWord() ) );

  // check word word button
  v_layout->addWidget( button = new QPushButton( "&Check word", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _checkWord() ) );

  // recheck button
  v_layout->addWidget( button = new QPushButton( "Recheck &Page", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _restart() ) );

  QFrame* frame;
  v_layout->addWidget( frame = new QFrame(this) );
  frame->setFrameShape( QFrame::HLine );
  
  // replace button
  v_layout->addWidget( button = new QPushButton( "&Replace", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _replace() ) );

  // replace button
  v_layout->addWidget( button = new QPushButton( "R&eplace all", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _replaceAll() ) );

  v_layout->addWidget( frame = new QFrame(this) );
  frame->setFrameShape( QFrame::HLine );
  
  // ignore button
  v_layout->addWidget( button = new QPushButton( "&Ignore", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _ignore() ) );

  // ignore button
  v_layout->addWidget( button = new QPushButton( "I&gnore all", this ) );
  connect( button, SIGNAL(clicked()), this, SLOT( _ignoreAll() ) );

  // state label_
  v_layout->addWidget( state_label_ = new QLabel( " ", this ), 1 );
  state_label_->setAlignment( Qt::AlignCenter );

  // change font
  QFont font( state_label_->font() );
  font.setBold( true );
  state_label_->setFont( font );
  
  // change color
  QPalette palette( state_label_->palette() );
  palette.setColor( QPalette::WindowText, Qt::red );
  state_label_->setPalette( palette );
  
  // retrieve available dictionaries
  const set<string>& dictionaries( interface().dictionaries() );
  for( set<string>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); iter++ )
  dict_combo_box_->addItem(iter->c_str() );

  connect( dict_combo_box_, SIGNAL( activated( const QString& ) ), this, SLOT( _selectDictionary( const QString& ) ) );
  dict_combo_box_->setEditText( interface().dictionary().c_str() );

  // retrieve available filters
  set<string> filters( interface().filters() );
  for( set<string>::iterator iter = filters.begin(); iter != filters.end(); iter++ )
  filter_combo_box_->addItem( iter->c_str() );
  connect( filter_combo_box_, SIGNAL( activated( const QString& ) ), this, SLOT( _selectFilter( const QString& ) ) );
  filter_combo_box_->setEditText( interface().filter().c_str() );

  // set text
  // check Editor seletion
  unsigned int index_begin( 0 );
  unsigned int index_end( editor().toPlainText().length() );
  if( editor().textCursor().hasSelection() )
  {
    index_begin = editor().textCursor().anchor();
    index_end = editor().textCursor().position();
    if( index_begin > index_end ) swap( index_begin, index_end );
  }

  // asign text
  if( !interface().setText( qPrintable( editor().toPlainText().mid( index_begin, index_end ) ) ) )
  { QtUtil::infoDialogExclusive( this, interface().error() ); } 

  // set TextEditor as ReadOnly
  read_only_editor_ = editor().isReadOnly();
  editor().setReadOnly( true );

  // size
  resize( 450, 330 );
  
  // start
  _restart();
  
}

//__________________________________________
SpellDialog::~SpellDialog( void )
{ Debug::Throw( "SpellDialog::~SpellDialog.\n" ); }

//__________________________________________
void SpellDialog::showFilter( const bool& value )
{
  Debug::Throw( "SpellDialog::showFilter.\n" );
  if( value ){
    filter_label_->show();
    filter_combo_box_->show();
  } else {
    filter_label_->hide();
    filter_combo_box_->hide();
  }
}

//____________________________________________________
void SpellDialog::_selectSuggestion()
{
  Debug::Throw( "SpellDialog::_selectSuggestion.\n" );

  // retrieve selected items
  QList<QListWidgetItem*> items( suggestion_list_box_->QListWidget::selectedItems() );
  if( items.empty() ) return;
  
  // try cast item
  replace_line_edit_->setText( items.front()->text() );

}

//____________________________________________________
void SpellDialog::_selectDictionary( const QString& dict )
{

  Debug::Throw( "SpellDialog::_SelectDictionary.\n" );

  if( !interface().setDictionary( qPrintable( dict ) ) )
  {
    QtUtil::infoDialogExclusive( this, interface().error() );
    return;
  }

  // update Options
  XmlOptions::get().set<string>( "DICTIONARY", qPrintable( dict ) );
  if( XmlOptions::file().size() ) XmlOptions::write();

  // restart
  _restart();

}

//____________________________________________________
void SpellDialog::_selectFilter( const QString& filter )
{

  Debug::Throw( "SpellDialog::_SelectFilter.\n" );

  if( !interface().setFilter( qPrintable( filter ) ) )
  {
    QtUtil::infoDialogExclusive( this, interface().error() );
    return;
  }

  // update Options
  XmlOptions::get().set<string>( "DICTIONARY_FILTER", qPrintable( filter ) );
  if( XmlOptions::file().size() ) XmlOptions::write();

  // restart
  _restart();

}

//____________________________________________________
void SpellDialog::_restart( void )
{
  Debug::Throw( "SpellDialog::_restart.\n" );

  if( !interface().reset() )
  {
    QtUtil::infoDialogExclusive( this, interface().error() );
    return;
  }

  // get next mispelled word
  state_label_->setText( "" );
  nextWord();

}

//____________________________________________________
void SpellDialog::_addWord( void )
{
  Debug::Throw( "SpellDialog::_addWord.\n" );

  if( !interface().addWord( interface().word() ) )
  {
    QtUtil::infoDialogExclusive( this, interface().error() );
    return;
  }

  emit ignoreWord( interface().word() ); 
  emit needUpdate();
  
  // get next mispelled word
  nextWord();

}

//____________________________________________________
void SpellDialog::_checkWord( void )
{
  Debug::Throw( "SpellDialog::_checkWord.\n" );

  // retrieve check word
  _displayWord( replace_line_edit_->text() );
  return;
}

//____________________________________________________
void SpellDialog::_ignore( void )
{

  Debug::Throw( "SpellDialog::_ignore.\n" );
  nextWord();

}

//____________________________________________________
void SpellDialog::_ignoreAll( void )
{

  Debug::Throw( "SpellDialog::_ignoreAll.\n" );
  string old_word( qPrintable( line_edit_->text() ) );
  ignored_words_.insert( old_word );
  _ignore();

}

//____________________________________________________
void SpellDialog::_replace( QListWidgetItem* )
{

  Debug::Throw( "SpellDialog::_replace.\n" );
  QString word( replace_line_edit_->text() );

  if( interface().replace( qPrintable( word ) ) )
  _replaceSelection( word );

  // parse next word
  nextWord();

}

//____________________________________________________
void SpellDialog::_replaceAll( void )
{

  Debug::Throw( "SpellDialog::_replaceAll.\n" );
  QString old_word( line_edit_->text() );
  QString new_word( replace_line_edit_->text() );
  replaced_words_.insert( make_pair( old_word, new_word ) );
  _replace();
  
}

//____________________________________________________
void SpellDialog::nextWord( void )
{

  Debug::Throw( "SpellDialog::nextWord.\n" );

  bool accepted( false );
  string word( "" );
  while( !accepted )
  {
    // get next word from interface
    if( !interface().nextWord() )
    {
      QtUtil::infoDialogExclusive( this, interface().error() );
      return;
    }

    // check for completed spelling
    word = interface().word();
    if( word.empty() ) 
    {
      Debug::Throw() << "SpellDialog::NextWord - empty word " << endl;
      break;
    }

    Debug::Throw() << "SpellDialog::NextWord - word: " << word << endl;
    
    // see if word is in ignore list
    if( ignored_words_.find( word ) != ignored_words_.end() ) continue;

    // see if word is in replace all list
    if( replaced_words_.find( word.c_str() ) != replaced_words_.end() )
    {
      
      // automatic replacement
      std::map< QString, QString >::iterator iter = replaced_words_.find( word.c_str() );
      _updateSelection( interface().position() + interface().offset(), word.size() );
      _replaceSelection( iter->second );
      interface().replace( qPrintable( iter->second ) );
      continue;
    }

    accepted = true;

  }

  if( accepted )
  {

    _updateSelection( interface().position() + interface().offset(), word.size() );
    _displayWord( word.c_str() );

  } else {

    // spelling completed. Clear everything
    line_edit_->clear();
    replace_line_edit_->clear();
    suggestion_list_box_->clear();
    state_label_->setText( "Spelling\n completed" );

  }

  return;

}

//__________________________________________
void SpellDialog::closeEvent( QCloseEvent *e )
{
  Debug::Throw( "SpellDialog::closeEvent.\n" );
  interface().saveWordList();
  editor().setReadOnly( read_only_editor_ );
  QDialog::closeEvent( e );
}

//_________________________________________________________________
void SpellDialog::_updateSelection( const unsigned int& index, const unsigned int& length )
{
  Debug::Throw() << "SpellDialog::_updateSelection - index=" << index << endl;
  QTextCursor cursor( editor().textCursor() );
  cursor.setPosition( index, QTextCursor::MoveAnchor );
  cursor.setPosition( index+length, QTextCursor::KeepAnchor );
  editor().setTextCursor( cursor );
  Debug::Throw( "SpellChecker::_UpdateSelection. done.\n" );

}

//_________________________________________________________________
void SpellDialog::_replaceSelection( const QString& word )
{

  Debug::Throw( "SpellDialog::_ReplaceSelection.\n" );
  editor().setReadOnly( false );
  editor().textCursor().insertText( word );
  editor().setReadOnly( true );

}

//_________________________________________________
void SpellDialog::_displayWord( const QString& word )
{

  Debug::Throw( "SpellDialog::_displayWord.\n" );

  // set text in line_editor
  line_edit_->setText( word );
  replace_line_edit_->setText( word );

  // clear list of suggestions
  suggestion_list_box_->clear();
  bool first = true;
  list<string> suggestions( interface().suggestions( qPrintable( word ) ) );
  for( list<string>::iterator iter = suggestions.begin(); iter != suggestions.end(); iter++ )
  {
    QListWidgetItem *item = new QListWidgetItem( suggestion_list_box_ );
    item->setText( iter->c_str() );
    if( first )
    {
      suggestion_list_box_->setItemSelected( item, true );
      first = false;
    }
  }
  
}
