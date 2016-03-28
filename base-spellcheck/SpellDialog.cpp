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

#include "Debug.h"
#include "DictionarySelectionButton.h"
#include "FilterSelectionButton.h"
#include "GridLayout.h"
#include "InformationDialog.h"
#include "LineEditor.h"
#include "SpellInterface.h"
#include "TextEditor.h"
#include "TreeView.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QTextCursor>

namespace SpellCheck
{
    //_______________________________________________
    SpellDialog::SpellDialog( TextEditor* parent, bool readOnly ):
        CustomDialog( parent, CloseButton|Separator ),
        editor_( parent ),
        readOnly_( readOnly )
    {
        Debug::Throw( "SpellCheck::SpellDialog::SpellDialog.\n" );

        // window title
        setWindowTitle( QString( readOnly ? tr( "Spell Check (read-only) - %1" ) : tr( "Spell Check - %1" ) ).arg( qApp->applicationName() ) );
        setOptionName( "SPELL_DIALOG" );

        // horizontal layout for suggestions and buttons
        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        hLayout->setSpacing(10);
        mainLayout().addLayout( hLayout, 1 );

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
        gridLayout->addWidget( sourceLabel_ = new QLabel( this ) );
        sourceLabel_->setTextInteractionFlags( Qt::TextSelectableByMouse );

        // replacement line editor
        gridLayout->addWidget( new QLabel( tr( "Replace with:" ), this ) );
        gridLayout->addWidget( replaceEditor_ = new LineEditor( this ) );
        if( readOnly ) replaceEditor_->setEnabled( false );

        gridLayout->setColumnStretch( 1, 1 );

        // suggestions
        vLayout->addWidget(  list_ = new TreeView( this ) );
        list_->setModel( &model_ );
        list_->header()->hide();

        // grid layout for dictionary and filter
        gridLayout = new GridLayout();
        gridLayout->setMargin( 0 );
        gridLayout->setSpacing(5);
        gridLayout->setMaxCount( 3 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );

        vLayout->addLayout( gridLayout, 0 );

        // dictionaries combobox
        QLabel* label;
        gridLayout->addWidget( label = new QLabel( tr( "Dictionary:" ), this ) );
        gridLayout->addWidget( dictionariesComboBox_ = new QComboBox( this ) );
        label->setBuddy( dictionariesComboBox_ );
        _updateDictionaries();


        connect( dictionariesComboBox_, SIGNAL(activated(QString)), SLOT(_selectDictionary(QString)) );

        // configuration
        DictionarySelectionButton* dictionarySelectionButton;
        gridLayout->addWidget( dictionarySelectionButton = new DictionarySelectionButton( this ) );
        connect( dictionarySelectionButton, SIGNAL(modified()), SLOT(_updateDictionaries()) );

        // filter combobox
        gridLayout->addWidget( filterLabel_ = new QLabel( tr( "Filter:" ), this ), 1, 0, 1, 1 );
        gridLayout->addWidget( filtersComboBox_ = new QComboBox( this ), 1, 1, 1, 1 );
        _updateFilters();
        filterLabel_->setBuddy( dictionariesComboBox_ );

        connect( filtersComboBox_, SIGNAL(activated(QString)), SLOT(_selectFilter(QString)) );

        // configuration
        FilterSelectionButton* filterSelectionButton;
        gridLayout->addWidget( filterSelectionButton = new FilterSelectionButton( this ) );
        connect( filterSelectionButton, SIGNAL(modified()), SLOT(_updateFilters()) );

        // stretch
        gridLayout->setColumnStretch( 1, 1 );

        // right vbox
        vLayout = new QVBoxLayout();
        vLayout->setMargin( 0 );
        vLayout->setSpacing(5);
        hLayout->addLayout( vLayout );

        // add word button
        vLayout->addWidget( addWordButton_ = new QPushButton( tr( "<< Add to Dictionary" ), this ) );
        addWordButton_->setToolTip( tr( "Add misspelled word to the current dictionnary" ) );
        connect( addWordButton_, SIGNAL(clicked()), SLOT(_addWord()) );


        // check word button
        vLayout->addWidget( checkWordButton_ = new QPushButton( tr( "Suggest" ), this ) );
        checkWordButton_->setToolTip( tr( "Suggest correct spelling for the word to use as a replacement" ) );
        connect( checkWordButton_, SIGNAL(clicked()), SLOT(_checkWord()) );

        // recheck button
        {
            QPushButton* button;
            vLayout->addWidget( button = new QPushButton( tr( "Recheck Page" ), this ) );
            connect( button, SIGNAL(clicked()), SLOT(_restart()) );
        }

        QFrame* frame;
        vLayout->addWidget( frame = new QFrame(this) );
        frame->setFrameShape( QFrame::HLine );

        // replace button
        vLayout->addWidget( replaceButton_ = new QPushButton( tr( "Replace" ), this ) );
        connect( replaceButton_, SIGNAL(clicked()), SLOT(_replace()) );
        if( readOnly ) replaceButton_->setVisible( false );

        // replace button
        vLayout->addWidget( replaceAllButton_ = new QPushButton( tr( "Replace All" ), this ) );
        connect( replaceAllButton_, SIGNAL(clicked()), SLOT(_replaceAll()) );
        if( readOnly ) replaceAllButton_->setVisible( false );

        vLayout->addWidget( frame = new QFrame(this) );
        frame->setFrameShape( QFrame::HLine );

        // ignore button
        vLayout->addWidget( ignoreButton_ = new QPushButton( tr( "Ignore" ), this ) );
        connect( ignoreButton_, SIGNAL(clicked()), SLOT(_ignore()) );

        // ignore button
        vLayout->addWidget( ignoreAllButton_ = new QPushButton( tr( "Ignore All" ), this ) );
        connect( ignoreAllButton_, SIGNAL(clicked()), SLOT(_ignoreAll()) );

        vLayout->addStretch( 1 );

        // connections
        connect( list_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(_selectSuggestion(QModelIndex)) );
        connect( list_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(_updateButtons()) );
        if( !readOnly ) { connect( list_, SIGNAL(activated(QModelIndex)), SLOT(_replace(QModelIndex)) ); }

        // set text
        // check Editor seletion
        unsigned int indexBegin( 0 );
        unsigned int indexEnd( editor_->toPlainText().length() );
        if( editor_->textCursor().hasSelection() )
        {
            indexBegin = editor_->textCursor().anchor();
            indexEnd = editor_->textCursor().position();
            if( indexBegin > indexEnd ) std::swap( indexBegin, indexEnd );
        }

        // asign text
        if( !interface().setText( editor_->toPlainText(), indexBegin, indexEnd ) )
        { InformationDialog( this, interface().error() ).exec(); }

        // set TextEditor as ReadOnly
        readOnlyEditor_ = editor_->isReadOnly();
        editor_->setReadOnly( true );

        Debug::Throw( "SpellCheck::SpellDialog::SpellDialog - done.\n" );

    }

    //__________________________________________
    void SpellDialog::showFilter( bool value )
    {
        Debug::Throw( "SpellCheck::SpellDialog::showFilter.\n" );
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
        Debug::Throw( "SpellCheck::SpellDialog::setDictionary.\n" );

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
        Debug::Throw( "SpellCheck::SpellDialog::setFilter.\n" );

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
        Debug::Throw( "SpellCheck::SpellDialog::_updateDictionaries.\n" );

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
        Debug::Throw( "SpellCheck::SpellDialog::_updateFilters.\n" );

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
        Debug::Throw( "SpellCheck::SpellDialog::_selectSuggestion.\n" );
        if( index.isValid() ) { replaceEditor_->setText( model_.get( index ) ); }
    }

    //____________________________________________________
    void SpellDialog::_selectDictionary( const QString& dictionary )
    {

        Debug::Throw( "SpellCheck::SpellDialog::_SelectDictionary.\n" );

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

        Debug::Throw( "SpellCheck::SpellDialog::_SelectFilter.\n" );

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
        Debug::Throw( "SpellCheck::SpellDialog::_restart.\n" );

        if( !interface().reset() )
        {
            InformationDialog( this, interface().error() ).exec();
            return;
        }

        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_addWord( void )
    {
        Debug::Throw( "SpellCheck::SpellDialog::_addWord.\n" );

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
        Debug::Throw( "SpellCheck::SpellDialog::_checkWord.\n" );

        // retrieve check word
        _displayWord( replaceEditor_->text() );
        return;
    }

    //____________________________________________________
    void SpellDialog::_ignore( void )
    {

        Debug::Throw( "SpellCheck::SpellDialog::_ignore.\n" );
        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_ignoreAll( void )
    {

        Debug::Throw( "SpellCheck::SpellDialog::_ignoreAll.\n" );
        interface().ignoreWord( sourceLabel_->text() );
        _ignore();

    }

    //____________________________________________________
    void SpellDialog::_replace( const QModelIndex& index )
    {

        Debug::Throw( "SpellCheck::SpellDialog::_replace.\n" );
        QModelIndex local( index );
        if( !local.isValid() ) local = list_->selectionModel()->currentIndex();
        if( !local.isValid() ) return;

        const QString word( model_.get( local ) );
        if( interface().replace( word ) ) _replaceSelection( word );

        // parse next word
        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_replaceAll( void )
    {

        Debug::Throw( "SpellCheck::SpellDialog::_replaceAll.\n" );
        QString oldWord( sourceLabel_->text() );
        QString newWord( replaceEditor_->text() );
        replacedWords_.insert( oldWord, newWord );
        _replace();

    }

    //____________________________________________________
    void SpellDialog::_updateButtons( void )
    {
        Debug::Throw( "SpellCheck::SpellDialog::_updateButtons.\n" );
        const bool hasSelection( list_->selectionModel()->hasSelection() );
        const bool hasWord( !sourceLabel_->text().isEmpty() );
        addWordButton_->setEnabled( hasWord );
        checkWordButton_->setEnabled( hasWord );
        ignoreButton_->setEnabled( hasWord );
        ignoreAllButton_->setEnabled( hasWord );
        replaceButton_->setEnabled( hasWord && hasSelection && !readOnly_ );
        replaceAllButton_->setEnabled( hasWord && hasSelection && !readOnly_ );
    }

    //____________________________________________________
    void SpellDialog::nextWord( void )
    {

        Debug::Throw( "SpellCheck::SpellDialog::nextWord.\n" );

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
                Debug::Throw() << "SpellCheck::SpellDialog::NextWord - empty word " << endl;
                break;
            }

            Debug::Throw() << "SpellCheck::SpellDialog::NextWord - word: " << word << endl;

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
            sourceLabel_->clear();
            replaceEditor_->clear();
            model_.clear();

            _updateButtons();

        }

        return;

    }

    //__________________________________________
    void SpellDialog::closeEvent( QCloseEvent *e )
    {
        Debug::Throw( "SpellCheck::SpellDialog::closeEvent.\n" );
        interface().saveWordList();
        editor_->setReadOnly( readOnlyEditor_ );
        QDialog::closeEvent( e );
    }

    //_________________________________________________________________
    void SpellDialog::_updateSelection( const unsigned int& index, const unsigned int& length )
    {
        Debug::Throw() << "SpellCheck::SpellDialog::_updateSelection - index=" << index << endl;
        QTextCursor cursor( editor_->textCursor() );
        cursor.setPosition( index, QTextCursor::MoveAnchor );
        cursor.setPosition( index+length, QTextCursor::KeepAnchor );
        editor_->setTextCursor( cursor );

    }

    //_________________________________________________________________
    void SpellDialog::_replaceSelection( const QString& word )
    {

        Debug::Throw() << "SpellCheck::SpellDialog::_ReplaceSelection - word: " << word << endl;
        editor_->setReadOnly( false );
        editor_->textCursor().insertText( word );
        editor_->setReadOnly( true );

    }

    //_________________________________________________
    void SpellDialog::_displayWord( const QString& word )
    {

        Debug::Throw( "SpellCheck::SpellDialog::_displayWord.\n" );

        // set text in line_editor
        sourceLabel_->setText( word );
        replaceEditor_->setText( word );

        // clear list of suggestions
        model_.clear();
        Model::List suggestions( interface().suggestions( word ) );
        model_.add( suggestions );

        // select the first suggestion, if any
        if( !suggestions.isEmpty() )
        {
            QModelIndex index( model_.index( suggestions.front() ) );
            list_->selectionModel()->select( index, QItemSelectionModel::Select|QItemSelectionModel::Rows );
            list_->selectionModel()->setCurrentIndex( index, QItemSelectionModel::Current );
        }

    }

    //_________________________________________________
    QVariant SpellDialog::Model::data( const QModelIndex &index, int role) const
    {

        // check index
        if( !contains( index ) ) return QVariant();

        // retrieve associated file info
        const QString& word( get(index) );

        // return text associated to file and column
        if( role == Qt::DisplayRole ) return word;
        else return QVariant();

    }

    //__________________________________________________________________
    QVariant SpellDialog::Model::headerData(int section, Qt::Orientation orientation, int role) const
    { return QVariant(); }
}
