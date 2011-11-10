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
\file SpellDialog.cpp
\brief spell checker popup dialog
\author  Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTextCursor>

#include "BaseIcons.h"
#include "GridLayout.h"
#include "Debug.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "SpellDialog.h"
#include "SpellInterface.h"
#include "TreeView.h"
#include "Util.h"

using namespace std;
using namespace SPELLCHECK;

//_______________________________________________
SpellDialog::SpellDialog( QTextEdit* parent, const bool& read_only ):
    BaseDialog( parent ),
    Counter( "SpellDialog" ),
    editor_( parent )
{
    Debug::Throw( "SpellDialog::SpellDialog.\n" );

    // window title
    setWindowTitle( read_only ? "Spell Check (read-only)" : "Spell Check" );
    setOptionName( "SPELL_DIALOG" );

    // create vbox layout
    QVBoxLayout* layout=new QVBoxLayout();
    layout->setMargin(10);
    layout->setSpacing(5);
    setLayout( layout );

    // horizontal layout for suggestions and buttons
    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->setMargin(0);
    h_layout->setSpacing(10);
    layout->addLayout( h_layout, 1 );

    // insert left vertical box
    QVBoxLayout *v_layout = new QVBoxLayout();
    v_layout->setMargin( 0 );
    v_layout->setSpacing(5);
    h_layout->addLayout( v_layout );

    // grid for text editors
    GridLayout *grid_layout = new GridLayout();
    grid_layout->setMargin( 0 );
    grid_layout->setSpacing(5);
    grid_layout->setMaxCount( 2 );
    v_layout->addLayout( grid_layout, 0 );

    // misspelled word line editor
    grid_layout->addWidget( new QLabel( "Misspelled word: ", this ) );
    grid_layout->addWidget( sourceEditor_ = new AnimatedLineEditor( this ) );
    sourceEditor_->setReadOnly( true );

    // replacement line editor
    grid_layout->addWidget( new QLabel( "Replace with: ", this ) );
    grid_layout->addWidget( replaceEditor_ = new AnimatedLineEditor( this ) );
    if( read_only ) replaceEditor_->setEnabled( false );

    grid_layout->setColumnStretch( 1, 1 );

    QLabel* label = new QLabel( "Suggestions: ", this );
    v_layout->addWidget( label, 0 );

    // suggestions
    v_layout->addWidget(  list_ = new TreeView( this ) );
    _list().setModel( &_model() );
    _list().header()->hide();

    connect( _list().selectionModel(), SIGNAL( currentChanged(const QModelIndex &, const QModelIndex &) ), SLOT( _selectSuggestion( const QModelIndex& ) ) );
    if( !read_only ) { connect( &_list(), SIGNAL( activated( const QModelIndex& ) ), SLOT( _replace( const QModelIndex& ) ) ); }

    // grid layout for dictionary and filter
    grid_layout = new GridLayout();
    grid_layout->setMargin( 0 );
    grid_layout->setSpacing(5);
    grid_layout->setMaxCount( 2 );
    v_layout->addLayout( grid_layout, 0 );

    // dictionaries combobox
    grid_layout->addWidget( new QLabel( "Dictionary: ", this ) );
    grid_layout->addWidget( dictionary_ = new QComboBox( this ) );

    const set<QString>& dictionaries( interface().dictionaries() );
    for( set<QString>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); ++iter )
        dictionary_->addItem(*iter );
    connect( dictionary_, SIGNAL( activated( const QString& ) ), SLOT( _selectDictionary( const QString& ) ) );

    // filter combobox
    grid_layout->addWidget( filterLabel_ = new QLabel( "Filter: ", this ) );
    grid_layout->addWidget( filter_ = new QComboBox( this ) );

    grid_layout->setColumnStretch( 1, 1 );

    set<QString> filters( interface().filters() );
    for( set<QString>::iterator iter = filters.begin(); iter != filters.end(); ++iter )
        filter_->addItem( *iter );
    connect( filter_, SIGNAL( activated( const QString& ) ), SLOT( _selectFilter( const QString& ) ) );

    // right vbox
    v_layout = new QVBoxLayout();
    v_layout->setMargin( 0 );
    v_layout->setSpacing(5);
    h_layout->addLayout( v_layout );

    // add word button
    QPushButton* button;
    v_layout->addWidget( button = new QPushButton( "&Add Word", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _addWord() ) );

    // check word button
    v_layout->addWidget( button = new QPushButton( "&Check Word", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _checkWord() ) );

    // recheck button
    v_layout->addWidget( button = new QPushButton( "Recheck &Page", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _restart() ) );

    QFrame* frame;
    v_layout->addWidget( frame = new QFrame(this) );
    frame->setFrameShape( QFrame::HLine );

    // replace button
    v_layout->addWidget( button = new QPushButton( "&Replace", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _replace() ) );
    if( read_only ) button->setEnabled( false );

    // replace button
    v_layout->addWidget( button = new QPushButton( "R&eplace All", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _replaceAll() ) );
    if( read_only ) button->setEnabled( false );

    v_layout->addWidget( frame = new QFrame(this) );
    frame->setFrameShape( QFrame::HLine );

    // ignore button
    v_layout->addWidget( button = new QPushButton( "&Ignore", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _ignore() ) );

    // ignore button
    v_layout->addWidget( button = new QPushButton( "I&gnore All", this ) );
    connect( button, SIGNAL(clicked()), SLOT( _ignoreAll() ) );

    // state label_
    v_layout->addWidget( stateLabel_ = new QLabel( " ", this ), 1 );
    stateLabel_->setAlignment( Qt::AlignCenter );

    // close button
    v_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&Close", this ) );
    connect( button, SIGNAL(clicked()), SLOT( close() ) );

    // change font
    QFont font( stateLabel_->font() );
    font.setWeight( QFont::Bold );
    stateLabel_->setFont( font );

    // change color
    QPalette palette( stateLabel_->palette() );
    palette.setColor( QPalette::WindowText, Qt::red );
    stateLabel_->setPalette( palette );

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
    if( !interface().setText( editor().toPlainText(), index_begin, index_end ) )
    { InformationDialog( this, interface().error() ).exec(); }

    // set TextEditor as ReadOnly
    readOnlyEditor_ = editor().isReadOnly();
    editor().setReadOnly( true );

    Debug::Throw( "SpellDialog::SpellDialog - done.\n" );

}

//__________________________________________
SpellDialog::~SpellDialog( void )
{ Debug::Throw( "SpellDialog::~SpellDialog.\n" ); }

//__________________________________________
void SpellDialog::showFilter( const bool& value )
{
    Debug::Throw( "SpellDialog::showFilter.\n" );
    if( value ){
        filterLabel_->show();
        filter_->show();
    } else {
        filterLabel_->hide();
        filter_->hide();
    }
}

//____________________________________________________
bool SpellDialog::setDictionary( const QString& dictionary )
{
    Debug::Throw( "SpellDialog::setDictionary.\n" );

    // find matching index
    int index( dictionary_->findText( dictionary ) );
    if( index < 0 )
    {
        QString buffer;
        QTextStream( &buffer ) << "invalid dictionary: " << dictionary;
        InformationDialog( this, buffer ).exec();
        return false;
    }

    // update interface
    if( !interface().setDictionary( dictionary ) )
    {
        InformationDialog( this, interface().error() ).exec();
        return false;
    }

    // select index
    dictionary_->setCurrentIndex( index );

    return true;

}

//____________________________________________________
bool SpellDialog::setFilter( const QString& filter )
{
    Debug::Throw( "SpellDialog::setFilter.\n" );

    // find matching index
    int index( filter_->findText( filter ) );
    if( index < 0 )
    {
        QString buffer;
        QTextStream( &buffer ) << "invalid dictionary: " << filter;
        InformationDialog( this, buffer ).exec();
        return false;
    }

    // update interface
    if( !interface().setFilter( filter ) )
    {
        InformationDialog( this, interface().error() ).exec();
        return false;
    }

    // select index
    filter_->setCurrentIndex( index );

    return true;

}

//____________________________________________________
void SpellDialog::_selectSuggestion( const QModelIndex& index )
{
    Debug::Throw( "SpellDialog::_selectSuggestion.\n" );
    if( index.isValid() ) { replaceEditor_->setText( _model().get( index ) ); }
}

//____________________________________________________
void SpellDialog::_selectDictionary( const QString& dictionary )
{

    Debug::Throw( "SpellDialog::_SelectDictionary.\n" );

    // see if changed
    if( interface().dictionary() == dictionary ) return;

    // try update interface
    if( !interface().setDictionary( dictionary ) )
    {
        InformationDialog( this, interface().error() ).exec();
        return;
    }

    // emit signal
    emit dictionaryChanged( interface().dictionary() );

    // restart
    _restart();

}

//____________________________________________________
void SpellDialog::_selectFilter( const QString& filter )
{

    Debug::Throw( "SpellDialog::_SelectFilter.\n" );

    // see if changed
    if( interface().filter() == filter ) return;

    // try update interface
    if( !interface().setFilter( filter ) )
    {
        InformationDialog( this, interface().error() ).exec();
        return;
    }

    // emit signal
    emit filterChanged( interface().filter() );

    // restart
    _restart();

}

//____________________________________________________
void SpellDialog::_restart( void )
{
    Debug::Throw( "SpellDialog::_restart.\n" );

    if( !interface().reset() )
    {
        InformationDialog( this, interface().error() ).exec();
        return;
    }

    // get next mispelled word
    stateLabel_->setText( "" );
    nextWord();

}

//____________________________________________________
void SpellDialog::_addWord( void )
{
    Debug::Throw( "SpellDialog::_addWord.\n" );

    if( !interface().addWord( interface().word() ) )
    {
        InformationDialog( this, interface().error() ).exec();
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
    _displayWord( replaceEditor_->text() );
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
    interface().ignoreWord( sourceEditor_->text() );
    _ignore();

}

//____________________________________________________
void SpellDialog::_replace( const QModelIndex& index )
{

    Debug::Throw( "SpellDialog::_replace.\n" );
    QModelIndex local( index );
    if( !local.isValid() ) local = _list().selectionModel()->currentIndex();
    if( !local.isValid() ) return;

    QString word( _model().get( index ) );

    if( interface().replace( word ) )
        _replaceSelection( word );

    // parse next word
    nextWord();

}

//____________________________________________________
void SpellDialog::_replaceAll( void )
{

    Debug::Throw( "SpellDialog::_replaceAll.\n" );
    QString old_word( sourceEditor_->text() );
    QString new_word( replaceEditor_->text() );
    replaced_words_.insert( make_pair( old_word, new_word ) );
    _replace();

}

//____________________________________________________
void SpellDialog::nextWord( void )
{

    Debug::Throw( "SpellDialog::nextWord.\n" );

    bool accepted( false );
    QString word( "" );
    while( !accepted )
    {
        // get next word from interface
        if( !interface().nextWord() )
        {
            InformationDialog( this, interface().error() ).exec();
            return;
        }

        // check for completed spelling
        word = interface().word();
        if( word.isEmpty() )
        {
            Debug::Throw() << "SpellDialog::NextWord - empty word " << endl;
            break;
        }

        Debug::Throw() << "SpellDialog::NextWord - word: " << word << endl;

        // see if word is in ignore list
        if( interface().isWordIgnored( word ) ) continue;

        // see if word is in replace all list
        if( replaced_words_.find( word ) != replaced_words_.end() )
        {

            // automatic replacement
            std::map< QString, QString >::iterator iter = replaced_words_.find( word );
            _updateSelection( interface().position() + interface().offset(), word.size() );
            _replaceSelection( iter->second );
            interface().replace( iter->second );
            continue;
        }

        accepted = true;

    }

    if( accepted )
    {

        _updateSelection( interface().position() + interface().offset(), word.size() );
        _displayWord( word );

    } else {

        // spelling completed. Clear everything
        sourceEditor_->clear();
        replaceEditor_->clear();
        _model().clear();
        stateLabel_->setText( "Spelling\n completed" );

    }

    return;

}

//__________________________________________
void SpellDialog::closeEvent( QCloseEvent *e )
{
    Debug::Throw( "SpellDialog::closeEvent.\n" );
    interface().saveWordList();
    editor().setReadOnly( readOnlyEditor_ );
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
    sourceEditor_->setText( word );
    replaceEditor_->setText( word );

    // clear list of suggestions
    _model().clear();
    Model::List suggestions( interface().suggestions( word ) );
    _model().add( suggestions );

    // would need to select first item

}

//_________________________________________________
QVariant SpellDialog::Model::data( const QModelIndex &index, int role) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // retrieve associated file info
    const QString& word( get(index) );

    // return text associated to file and column
    if( role == Qt::DisplayRole ) {

        switch( index.column() )
        {
            case NAME: return word;
            default: return QVariant();
        }
    }

    return QVariant();

}

//__________________________________________________________________
QVariant SpellDialog::Model::headerData(int section, Qt::Orientation orientation, int role) const
{ return QVariant(); }
