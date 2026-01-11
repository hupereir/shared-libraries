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
#include "QtUtil.h"
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
        Dialog( parent, CloseButton|Separator ),
        editor_( parent ),
        readOnly_( readOnly )
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::SpellDialog.\n") );

        // window title
        setWindowTitle( readOnly ? tr( "Spell Check (read-only)" ) : tr( "Spell Check" ) );
        setOptionName( QStringLiteral("SPELL_DIALOG") );

        // horizontal layout for suggestions and buttons
        auto hLayout = new QHBoxLayout;
        QtUtil::setMargin(hLayout, 0);
        hLayout->setSpacing(10);
        mainLayout().addLayout( hLayout, 1 );

        // insert left vertical box
        auto vLayout = new QVBoxLayout;
        QtUtil::setMargin(vLayout, 0);
        vLayout->setSpacing(5);
        hLayout->addLayout( vLayout );

        // grid for text editors
        auto gridLayout = new GridLayout;
        QtUtil::setMargin(gridLayout, 0);
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
        gridLayout = new GridLayout;
        QtUtil::setMargin(gridLayout, 0);
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

        connect( dictionariesComboBox_, &QComboBox::textActivated, this, &SpellDialog::_selectDictionary );

        // configuration
        DictionarySelectionButton* dictionarySelectionButton;
        gridLayout->addWidget( dictionarySelectionButton = new DictionarySelectionButton( this ) );
        connect( dictionarySelectionButton, &DictionarySelectionButton::modified, this, &SpellDialog::_updateDictionaries );

        // filter combobox
        gridLayout->addWidget( filterLabel_ = new QLabel( tr( "Filter:" ), this ), 1, 0, 1, 1 );
        gridLayout->addWidget( filtersComboBox_ = new QComboBox( this ), 1, 1, 1, 1 );
        _updateFilters();
        filterLabel_->setBuddy( dictionariesComboBox_ );

        connect( filtersComboBox_, &QComboBox::textActivated, this, &SpellDialog::_selectFilter );

        // configuration
        FilterSelectionButton* filterSelectionButton;
        gridLayout->addWidget( filterSelectionButton = new FilterSelectionButton( this ) );
        connect( filterSelectionButton, &FilterSelectionButton::modified, this, &SpellDialog::_updateFilters );

        // stretch
        gridLayout->setColumnStretch( 1, 1 );

        // right vbox
        vLayout = new QVBoxLayout;
        QtUtil::setMargin(vLayout, 0);
        vLayout->setSpacing(5);
        hLayout->addLayout( vLayout );

        // add word button
        vLayout->addWidget( addWordButton_ = new QPushButton( tr( "<< Add to Dictionary" ), this ) );
        addWordButton_->setToolTip( tr( "Add misspelled word to the current dictionnary" ) );
        connect( addWordButton_, &QAbstractButton::clicked, this, &SpellDialog::_addWord );


        // check word button
        vLayout->addWidget( checkWordButton_ = new QPushButton( tr( "Suggest" ), this ) );
        checkWordButton_->setToolTip( tr( "Suggest correct spelling for the word to use as a replacement" ) );
        connect( checkWordButton_, &QAbstractButton::clicked, this, &SpellDialog::_checkWord );

        // recheck button
        {
            QPushButton* button;
            vLayout->addWidget( button = new QPushButton( tr( "Recheck Page" ), this ) );
            connect( button, &QAbstractButton::clicked, this, &SpellDialog::_restart );
        }

        QFrame* frame;
        vLayout->addWidget( frame = new QFrame(this) );
        frame->setFrameShape( QFrame::HLine );

        // replace button
        vLayout->addWidget( replaceButton_ = new QPushButton( tr( "Replace" ), this ) );
        connect( replaceButton_, &QPushButton::clicked, [this](bool){ _replace(); } );
        if( readOnly ) replaceButton_->setVisible( false );

        // replace button
        vLayout->addWidget( replaceAllButton_ = new QPushButton( tr( "Replace All" ), this ) );
        connect( replaceAllButton_, &QAbstractButton::clicked, this, &SpellDialog::_replaceAll );
        if( readOnly ) replaceAllButton_->setVisible( false );

        vLayout->addWidget( frame = new QFrame(this) );
        frame->setFrameShape( QFrame::HLine );

        // ignore button
        vLayout->addWidget( ignoreButton_ = new QPushButton( tr( "Ignore" ), this ) );
        connect( ignoreButton_, &QAbstractButton::clicked, this, &SpellDialog::_ignore );

        // ignore button
        vLayout->addWidget( ignoreAllButton_ = new QPushButton( tr( "Ignore All" ), this ) );
        connect( ignoreAllButton_, &QAbstractButton::clicked, this, &SpellDialog::_ignoreAll );

        vLayout->addStretch( 1 );

        // connections
        connect( list_->selectionModel(), &QItemSelectionModel::currentChanged, this, &SpellDialog::_selectSuggestion );
        connect( list_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SpellDialog::_updateButtons );
        if( !readOnly ) { connect( list_, &QAbstractItemView::activated, this, &SpellDialog::_replace ); }

        // set text
        // check Editor seletion
        int indexBegin( 0 );
        int indexEnd( editor_->toPlainText().length() );
        if( editor_->textCursor().hasSelection() )
        {
            indexBegin = editor_->textCursor().anchor();
            indexEnd = editor_->textCursor().position();
            if( indexBegin > indexEnd ) std::swap( indexBegin, indexEnd );
        }

        // asign text
        if( !interface_.setText( editor_->toPlainText(), indexBegin, indexEnd ) )
        { InformationDialog( this, interface_.error() ).exec(); }

        // set TextEditor as ReadOnly
        readOnlyEditor_ = editor_->isReadOnly();
        editor_->setReadOnly( true );

        // connection
        connect( &closeButton(), &QAbstractButton::clicked, this, &SpellDialog::_saveWordList );
        connect( &closeButton(), &QAbstractButton::clicked, this, &SpellDialog::_restoreReadOnly );

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::SpellDialog - done.\n") );

    }

    //__________________________________________
    void SpellDialog::showFilter( bool value )
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::showFilter.\n") );
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
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::setDictionary.\n") );

        // find matching index
        int index( dictionariesComboBox_->findText( dictionary ) );
        if( index < 0 )
        {
            InformationDialog( this, tr( "Invalid dictionary: %1" ).arg( dictionary ) ).exec();
            return false;
        }

        // update interface
        if( !interface_.setDictionary( dictionary ) )
        {
            InformationDialog( this, interface_.error() ).exec();
            return false;
        }

        // select index
        dictionariesComboBox_->setCurrentIndex( index );

        return true;

    }

    //____________________________________________________
    bool SpellDialog::setFilter( const QString& filter )
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::setFilter.\n") );

        // find matching index
        int index( filtersComboBox_->findText( filter ) );
        if( index < 0 )
        {
            InformationDialog( this, tr( "Invalid filter: %1" ).arg( filter ) ).exec();
            return false;
        }

        // update interface
        if( !interface_.setFilter( filter ) )
        {
            InformationDialog( this, interface_.error() ).exec();
            return false;
        }

        // select index
        filtersComboBox_->setCurrentIndex( index );

        return true;

    }

    //___________________________________________
    void SpellDialog::_updateDictionaries()
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_updateDictionaries.\n") );

        // store selection
        const auto selection( dictionariesComboBox_->currentText() );

        // read list of disabled dictionaries
        const auto disabledDictionaries( QString( XmlOptions::get().raw( QStringLiteral("SPELLCHECK_DISABLED_DICTIONARIES") ) ).split( QLatin1Char(' ') ) );

        // clear combobox
        dictionariesComboBox_->clear();

        // get dictionary list and populate combobox
        for( const auto& dictionary:interface_.dictionaries() )
        { if( !disabledDictionaries.contains( dictionary ) ) dictionariesComboBox_->addItem( dictionary ); }

        // restore default value
        dictionariesComboBox_->setCurrentIndex( dictionariesComboBox_->findText( selection ) );

    }

    //___________________________________________
    void SpellDialog::_updateFilters()
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_updateFilters.\n") );

        // store selection
        const auto selection( filtersComboBox_->currentText() );

        // read list of disabled filters
        const auto disabledFilters( QString( XmlOptions::get().raw( QStringLiteral("SPELLCHECK_DISABLED_FILTERS") ) ).split( QLatin1Char(' ') ) );

        // clear combobox
        filtersComboBox_->clear();

        // get dictionary list and populate combobox
        for( const auto& filter:interface_.filters() )
        { if( !disabledFilters.contains( filter ) ) filtersComboBox_->addItem( filter ); }

        // restore default value
        filtersComboBox_->setCurrentIndex( filtersComboBox_->findText( selection ) );

    }

    //____________________________________________________
    void SpellDialog::_selectSuggestion( const QModelIndex& index )
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_selectSuggestion.\n") );
        if( index.isValid() ) { replaceEditor_->setText( model_.get( index ) ); }
    }

    //____________________________________________________
    void SpellDialog::_selectDictionary( const QString& dictionary )
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_SelectDictionary.\n") );

        // see if changed
        if( interface_.dictionary() == dictionary ) return;

        // try update interface
        if( !interface_.setDictionary( dictionary ) )
        {
            InformationDialog( this, interface_.error() ).exec();
            return;
        }

        // emit signal
        emit dictionaryChanged( interface_.dictionary() );

        // restart
        _restart();

    }

    //____________________________________________________
    void SpellDialog::_selectFilter( const QString& filter )
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_SelectFilter.\n") );

        // see if changed
        if( interface_.filter() == filter ) return;

        // try update interface
        if( !interface_.setFilter( filter ) )
        {
            InformationDialog( this, interface_.error() ).exec();
            return;
        }

        // emit signal
        emit filterChanged( interface_.filter() );

        // restart
        _restart();

    }

    //____________________________________________________
    void SpellDialog::_restart()
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_restart.\n") );

        if( !interface_.reset() )
        {
            InformationDialog( this, interface_.error() ).exec();
            return;
        }

        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_addWord()
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_addWord.\n") );

        if( !interface_.addWord( interface_.word() ) )
        {
            InformationDialog( this, interface_.error() ).exec();
            return;
        }

        emit ignoreWord( interface_.word() );
        emit needUpdate();

        // get next mispelled word
        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_checkWord()
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_checkWord.\n") );

        // retrieve check word
        _displayWord( replaceEditor_->text() );
        return;
    }

    //____________________________________________________
    void SpellDialog::_ignore()
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_ignore.\n") );
        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_ignoreAll()
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_ignoreAll.\n") );
        interface_.ignoreWord( sourceLabel_->text() );
        _ignore();

    }

    //____________________________________________________
    void SpellDialog::_replace( const QModelIndex& index )
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_replace.\n") );
        QModelIndex local( index );
        if( !local.isValid() ) local = list_->selectionModel()->currentIndex();
        if( !local.isValid() ) return;

        const auto word( model_.get( local ) );
        if( interface_.replace( word ) ) _replaceSelection( word );

        // parse next word
        nextWord();

    }

    //____________________________________________________
    void SpellDialog::_replaceAll()
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_replaceAll.\n") );
        QString oldWord( sourceLabel_->text() );
        QString newWord( replaceEditor_->text() );
        replacedWords_.insert( oldWord, newWord );
        _replace();

    }

    //____________________________________________________
    void SpellDialog::_updateButtons()
    {
        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_updateButtons.\n") );
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
    void SpellDialog::nextWord()
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::nextWord.\n") );

        bool accepted( false );
        QString word;
        while( !accepted )
        {
            // get next word from interface
            if( !interface_.nextWord() )
            {
                InformationDialog( this, interface_.error() ).exec();
                return;
            }

            // check for completed spelling
            word = interface_.word();
            if( word.isEmpty() )
            {
                Debug::Throw() << "SpellCheck::SpellDialog::NextWord - empty word " << Qt::endl;
                break;
            }

            Debug::Throw() << "SpellCheck::SpellDialog::NextWord - word: " << word << Qt::endl;

            // see if word is in ignore list
            if( interface_.isWordIgnored( word ) ) continue;

            // see if word is in replace all list
            if( replacedWords_.find( word ) != replacedWords_.end() )
            {

                // automatic replacement
                QHash< QString, QString >::iterator iter = replacedWords_.find( word );
                _updateSelection( interface_.position() + interface_.offset(), word.size() );
                _replaceSelection( iter.value() );
                interface_.replace( iter.value() );
                continue;
            }

            accepted = true;

        }

        if( accepted )
        {

            _updateSelection( interface_.position() + interface_.offset(), word.size() );
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
        Debug::Throw( 0, QStringLiteral("SpellCheck::SpellDialog::closeEvent.\n") );
        _restoreReadOnly();
        _saveWordList();
        QDialog::closeEvent( e );
    }

    //__________________________________________
    void SpellDialog::_saveWordList()
    { interface_.saveWordList(); }

    //__________________________________________
    void SpellDialog::_restoreReadOnly()
    { editor_->setReadOnly( readOnlyEditor_ ); }

    //_________________________________________________________________
    void SpellDialog::_updateSelection( int index, int length )
    {
        Debug::Throw() << "SpellCheck::SpellDialog::_updateSelection - index=" << index << Qt::endl;
        auto cursor( editor_->textCursor() );
        cursor.setPosition( index, QTextCursor::MoveAnchor );
        cursor.setPosition( index+length, QTextCursor::KeepAnchor );
        editor_->setTextCursor( cursor );

    }

    //_________________________________________________________________
    void SpellDialog::_replaceSelection( const QString& word )
    {

        Debug::Throw() << "SpellCheck::SpellDialog::_ReplaceSelection - word: " << word << Qt::endl;
        editor_->setReadOnly( false );
        editor_->textCursor().insertText( word );
        editor_->setReadOnly( true );

    }

    //_________________________________________________
    void SpellDialog::_displayWord( const QString& word )
    {

        Debug::Throw( QStringLiteral("SpellCheck::SpellDialog::_displayWord.\n") );

        // set text in line_editor
        sourceLabel_->setText( word );
        replaceEditor_->setText( word );

        // clear list of suggestions
        model_.clear();
        const auto suggestions( interface_.suggestions( word ) );
        model_.add( Model::List::fromList( suggestions ) );

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
        if( !contains( index ) ) return {};

        // retrieve associated file info
        const QString& word( get(index) );

        // return text associated to file and column
        if( role == Qt::DisplayRole ) return word;
        else return {};

    }

    //__________________________________________________________________
    QVariant SpellDialog::Model::headerData(int, Qt::Orientation, int) const
    { return {}; }
}
