#ifndef SpellDialog_h
#define SpellDialog_h

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

#include "Counter.h"
#include "CustomDialog.h"
#include "ListModel.h"
#include "SpellInterface.h"
#include "TextPosition.h"

#include <QComboBox>
#include <QLabel>
#include <QHash>

class TextEditor;
class LineEditor;
class TreeView;

namespace SpellCheck
{

    //* spell checker popup dialog
    class SpellDialog: public CustomDialog
    {

        //* Qt meta object declaration
        Q_OBJECT

        public:

        //* constructor
        SpellDialog( TextEditor*, bool = false );

        //* set read-only (enable/disable modification buttons)

        //* toggle filter combo box visibility
        virtual void showFilter( bool value );

        //* spell interface
        virtual SpellInterface& interface( void )
        { return interface_; }

        //* editor
        TextEditor &editor( void )
        { return *editor_; }

        //* dictionary
        bool setDictionary( const QString& dict );

        //* dictionary
        QString dictionary( void ) const
        { return dictionariesComboBox_->itemText( dictionariesComboBox_->currentIndex() ); }

        //* filter
        bool setFilter( const QString& filter );

        //* dictionary
        QString filter( void ) const
        { return filtersComboBox_->itemText( filtersComboBox_->currentIndex() ); }

        //* go to next word to be checked
        void nextWord( void );

        Q_SIGNALS:

        //* ignore word
        void ignoreWord( const QString& );

        //* need update
        void needUpdate( void );

        //* filter has changed
        void filterChanged( QString );

        //* dictionary has changed
        void dictionaryChanged( QString );

        protected Q_SLOTS:

        //* update dictionaries
        void _updateDictionaries( void );

        //* update filters
        void _updateFilters( void );

        //* select suggestion, update replaceEditor_
        virtual void _selectSuggestion( const QModelIndex& );

        //* select suggestion, update replaceEditor_
        virtual void _addWord( void );

        //* check word in Replace Editor
        virtual void _checkWord( void );

        //* select dictionary
        virtual void _selectDictionary( const QString& dict );

        //* select dictionary
        virtual void _selectFilter( const QString& filter );

        //* recheck text from start
        /** initial begin/end are reset to the full text */
        virtual void _restart( void );

        //* ignore
        virtual void _ignore( void );

        //* ignore all
        virtual void _ignoreAll( void );

        //* replace
        virtual void _replace( const QModelIndex& index = QModelIndex() );

        //* replace All
        virtual void _replaceAll( void );

        //* update buttons
        virtual void _updateButtons( void );

        protected:

        //* close
        virtual void closeEvent( QCloseEvent *e );

        private:

        //* qlistview for object IconCaches
        class Model: public ListModel<QString>
        {

            public:

            //* number of columns
            enum { nColumns = 1 };

            //*@name methods reimplemented from base class
            //@{

            // return data for a given index
            virtual QVariant data(const QModelIndex &index, int role) const;

            //* header data
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

            //* number of columns for a given index
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
            { return nColumns; }

            //@}

            protected:

            virtual void _sort(int, Qt::SortOrder)
            { return; }

        };

        //* update text editor selection
        void _updateSelection( const unsigned int&, const unsigned int& );

        //* replace text editor selection
        void _replaceSelection( const QString& );

        //* update suggestion list and editor for words
        void _displayWord( const QString& word );

        //* spell checking completed
        void _completed( void );

        //* spell interface
        SpellInterface interface_;

        //* text editor
        TextEditor* editor_ = nullptr;

        //* read only flag
        bool readOnly_ = false;

        //* initial readonly state
        bool readOnlyEditor_ = false;

        //* line editor for original word
        QLabel *sourceLabel_ = nullptr;

        //* line editor for text replacement
        LineEditor *replaceEditor_ = nullptr;

        //* model
        Model model_;

        //* listbox for suggestions
        TreeView *list_ = nullptr;

        //* combo box for dictionary
        QComboBox *dictionariesComboBox_ = nullptr;

        //* filter label
        QLabel *filterLabel_ = nullptr;

        //* combo box for filter
        QComboBox *filtersComboBox_ = nullptr;

        //* buttons
        QPushButton* addWordButton_ = nullptr;
        QPushButton* checkWordButton_ = nullptr;
        QPushButton* ignoreButton_ = nullptr;
        QPushButton* ignoreAllButton_ = nullptr;
        QPushButton* replaceButton_ = nullptr;
        QPushButton* replaceAllButton_ = nullptr;

        //* list of automatic replace words
        QHash< QString, QString > replacedWords_;

    };
};

#endif
