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

#include "SpellDialog.h"

#include "BaseIcons.h"
#include "Debug.h"
#include "DictionarySelectionButton.h"
#include "FilterSelectionButton.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "SpellInterface.h"
#include "TreeView.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QTextCursor>

namespace SPELLCHECK
{
    //_______________________________________________
    SpellDialog::SpellDialog( TextEditor* parent, const bool& read_only ):
        BaseDialog( parent ),
        Counter( "SpellDialog" ),
        editor_( parent )
    {
        Debug::Throw( "SpellDialog::SpellDialog.\n" );

        // window title
        setWindowTitle( QString( read_only ? tr( "Spell Check (read-only) - %1" ) : tr( "Spell Check - %1" ) ).arg( qApp->applicationName() ) );
        setOptionName( "SPELL_DIALOG" );

        // create vbox layout
        QVBoxLayout* layout=new QVBoxLayout();
        layout->setMargin(10);
        layout->setSpacing(5);
        setLayout( layout );

        // horizontal layout for suggestions and buttons
        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        hLayout->setSpacing(10);
        layout->addLayout( hLayout, 1 );

        // insert left vertical box
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->setMargin( 0 );
        vLayout->setSpacing(5);
        hLayout->addLayout( vLayout );

        // grid for text editors
        GridLayout *gridLayout = new GridLayout();
        gridLayout->setMargin( 0 );
        gridLayout->setSpacing(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        vLayout->addLayout( gridLayout, 0 );

        // misspelled word line editor
        gridLayout->addWidget( new QLabel( tr( "Misspelled word:" ), this ) );
        gridLayout->addWidget( sourceEditor_ = new AnimatedLineEditor( this ) );
        sourceEditor_->setReadOnly( true );

        // replacement line editor
        gridLayout->addWidget( new QLabel( tr( "Replace with:" ), this ) );
        gridLayout->addWidget( replaceEditor_ = new AnimatedLineEditor( this ) );
        if( read_only ) replaceEditor_->setEnabled( false );

        gridLayout->setColumnStretch( 1, 1 );

        QLabel* label = new QLabel( tr( "Suggestions:" ), this );
        vLayout->addWidget( label, 0 );

        // suggestions
        vLayout->addWidget(  list_ = new TreeView( this ) );
        _list().setModel( &_model() );
        _list().header()->hide();

        connect( _list().selectionModel(), SIGNAL( currentChanged(const QModelIndex &, const QModelIndex &) ), SLOT( _selectSuggestion( const QModelIndex& ) ) );
        if( !read_only ) { connect( &_list(), SIGNAL( activated( const QModelIndex& ) ), SLOT( _replace( const QModelIndex& ) ) ); }

        // grid layout for dictionary and filter
        gridLayout = new GridLayout();
        gridLayout->setMargin( 0 );
        gridLayout->setSpacing(5);
        gridLayout->setMaxCount( 3 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );

        vLayout->addLayout( gridLayout, 0 );

        // dictionaries combobox
        gridLayout->addWidget( label = new QLabel( tr( "Dictionary:" ), this ) );
        gridLayout->addWidget( dictionariesComboBox_ = new QComboBox( this ) );
        _updateDictionaries();

        connect( dictionariesComboBox_, SIGNAL( activated( const QString& ) ), SLOT( _selectDictionary( const QString& ) ) );

        // configuration
        DictionarySelectionButton* dictionarySelectionButton;
        gridLayout->addWidget( dictionarySelectionButton = new DictionarySelectionButton( this ) );
        connect( dictionarySelectionButton, SIGNAL( modified() ), SLOT( _updateDictionaries() ) );

        // filter combobox
        gridLayout->addWidget( filterLabel_ = new QLabel( tr( "Filter:" ), this ), 1, 0, 1, 1 );
        gridLayout->addWidget( filtersComboBox_ = new QComboBox( this ), 1, 1, 1, 1 );
        _updateFilters();

        connect( filtersComboBox_, SIGNAL( activated( const QString& ) ), SLOT( _selectFilter( const QString& ) ) );

        // configuration
        FilterSelectionButton* filterSelectionButton;
        gridLayout->addWidget( filterSelectionButton = new FilterSelectionButton( this ) );
        connect( filterSelectionButton, SIGNAL( modified() ), SLOT( _updateFilters() ) );

        // stretch
        gridLayout->setColumnStretch( 1, 1 );

        // right vbox
        vLayout = new QVBoxLayout();
        vLayout->setMargin( 0 );
        vLayout->setSpacing(5);
        hLayout->addLayout( vLayout );

        // add word button
        QPushButton* button;
        vLayout->addWidget( button = new QPushButton( tr( "Add Word" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _addWord() ) );

        // check word button
        vLayout->addWidget( button = new QPushButton( tr( "Check Word" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _checkWord() ) );

        // recheck button
        vLayout->addWidget( button = new QPushButton( tr( "Recheck Page" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _restart() ) );

        QFrame* frame;
        vLayout->addWidget( frame = new QFrame(this) );
        frame->setFrameShape( QFrame::HLine );

        // replace button
        vLayout->addWidget( button = new QPushButton( tr( "Replace" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _replace() ) );
        if( read_only ) button->setEnabled( false );

        // replace button
        vLayout->addWidget( button = new QPushButton( tr( "Replace All" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _replaceAll() ) );
        if( read_only ) button->setEnabled( false );

        vLayout->addWidget( frame = new QFrame(this) );
        frame->setFrameShape( QFrame::HLine );

        // ignore button
        vLayout->addWidget( button = new QPushButton( tr( "Ignore" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _ignore() ) );

        // ignore button
        vLayout->addWidget( button = new QPushButton( tr( "Ignore All" ), this ) );
        connect( button, SIGNAL(clicked()), SLOT( _ignoreAll() ) );

        // state label_
        vLayout->addWidget( stateLabel_ = new QLabel( " ", this ), 1 );
        stateLabel_->setAlignment( Qt::AlignCenter );

        // close button
        vLayout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), tr( "Close" ), this ) );
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
            if( index_begin > index_end ) std::swap( index_begin, index_end );
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
            filtersComboBox_->show();
        } else {
            filterLabel_->hide();
            filtersComboBox_->hide();
        }
    }

    //____________________________________________________
    bool SpellDialog::setDictionary( const QString& dictionary )
    {
        Debug::Throw( "SpellDialog::setDictionary.\n" );

        // find matching index
        int index( dictionariesComboBox_->findText( dictionary ) );
        if( index < 0 )
        {
            InformationDialog( this, QString( tr( "Invalid dictionary: %1" ) ).arg( dictionary ) ).exec();
            return false;
        }

        // update interface
        if( !interface().setDictionary( dictionary ) )
        {
            InformationDialog( this, interface().error() ).exec();
            return false;
        }

        // select index
        dictionariesComboBox_->setCurrentIndex( index );

        return true;

    }

    //____________________________________________________
    bool SpellDialog::setFilter( const QString& filter )
    {
        Debug::Throw( "SpellDialog::setFilter.\n" );

        // find matching index
        int index( filtersComboBox_->findText( filter ) );
        if( index < 0 )
        {
            InformationDialog( this, QString( tr( "Invalid filter: %1" ) ).arg( filter ) ).exec();
            return false;
        }

        // update interface
        if( !interface().setFilter( filter ) )
        {
            InformationDialog( this, interface().error() ).exec();
            return false;
        }

        // select index
        filtersComboBox_->setCurrentIndex( index );

        return true;

    }


    //___________________________________________
    void SpellDialog::_updateDictionaries( void )
    {
        Debug::Throw( "SpellDialog::_updateDictionaries.\n" );

        // store selection
        const QString selection( dictionariesComboBox_->currentText() );

        // read list of disabled dictionaries
        const QStringList disabledDictionaries( QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_DICTIONARIES" ) ).split( " " ) );

        // clear combobox
        dictionariesComboBox_->clear();

        // get dictionary list and populate combobox
        foreach( const QString& dictionary, interface().dictionaries() )
        { if( !disabledDictionaries.contains( dictionary ) ) dictionariesComboBox_->addItem( dictionary ); }

        // restore default value
        dictionariesComboBox_->setCurrentIndex( dictionariesComboBox_->findText( selection ) );

    }

    //___________________________________________
    void SpellDialog::_updateFilters( void )
    {
        Debug::Throw( "SpellDialog::_updateFilters.\n" );

        // store selection
        const QString selection( filtersComboBox_->currentText() );

        // read list of disabled filters
        const QStringList disabledFilters( QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_FILTERS" ) ).split( " " ) );

        // clear combobox
        filtersComboBox_->clear();

        // get dictionary list and populate combobox
        foreach( const QString& filter, interface().filters() )
        { if( !disabledFilters.contains( filter ) ) filtersComboBox_->addItem( filter ); }

        // restore default value
        filtersComboBox_->setCurrentIndex( filtersComboBox_->findText( selection ) );

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
        QString oldWord( sourceEditor_->text() );
        QString newWord( replaceEditor_->text() );
        replacedWords_.insert( oldWord, newWord );
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
            if( replacedWords_.find( word ) != replacedWords_.end() )
            {

                // automatic replacement
                QHash< QString, QString >::iterator iter = replacedWords_.find( word );
                _updateSelection( interface().position() + interface().offset(), word.size() );
                _replaceSelection( iter.value() );
                interface().replace( iter.value() );
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
            stateLabel_->setText( tr( "Spelling completed" ) );

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
}
