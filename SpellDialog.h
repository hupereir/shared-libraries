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

#ifndef SpellDialog_h
#define SpellDialog_h

/*!
\file SpellDialog.h
\brief spell checker popup dialog
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "BaseDialog.h"
#include "Counter.h"
#include "ListModel.h"
#include "AnimatedLineEditor.h"
#include "SpellInterface.h"
#include "TextPosition.h"

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>

#include <cassert>
#include <map>
#include <set>

class TreeView;

namespace SPELLCHECK
{

    //! spell checker popup dialog
    class SpellDialog: public BaseDialog, public Counter
    {

        //! Qt meta object declaration
        Q_OBJECT

        public:

        //! constructor
        SpellDialog( QTextEdit* parent, const bool& read_only = false );

        //! destructor
        virtual ~SpellDialog();

        //! set read-only (enable/disable modification buttons)

        //! toggle filter combo box visibility
        virtual void showFilter( const bool& value );

        //! spell interface
        virtual SpellInterface& interface( void )
        { return interface_; }

        //! editor
        QTextEdit &editor( void )
        {
            assert( editor_ );
            return *editor_;
        }

        //! dictionary
        bool setDictionary( const QString& dict );

        //! dictionary
        QString dictionary( void ) const
        { return dictionary_->itemText( dictionary_->currentIndex() ); }

        //! filter
        bool setFilter( const QString& filter );

        //! dictionary
        QString filter( void ) const
        { return filter_->itemText( filter_->currentIndex() ); }

        //! go to next word to be checked
        void nextWord( void );

        signals:

        //! ignore word
        void ignoreWord( const QString& );

        //! need update
        void needUpdate( void );

        //! filter has changed
        void filterChanged( QString );

        //! dictionary has changed
        void dictionaryChanged( QString );

        protected slots:

        //! select suggestion, update replaceEditor_
        virtual void _selectSuggestion( const QModelIndex& );

        //! select suggestion, update replaceEditor_
        virtual void _addWord( void );

        //! check word in Replace Editor
        virtual void _checkWord( void );

        //! select dictionary
        virtual void _selectDictionary( const QString& dict );

        //! select dictionary
        virtual void _selectFilter( const QString& filter );

        //! recheck text from start
        /*! initial begin/end are reset to the full text */
        virtual void _restart( void );

        //! ignore
        virtual void _ignore( void );

        //! ignore all
        virtual void _ignoreAll( void );

        //! replace
        virtual void _replace( const QModelIndex& index = QModelIndex() );

        //! replace All
        virtual void _replaceAll( void );

        protected:

        //! close
        virtual void closeEvent( QCloseEvent *e );

        private:

        //! qlistview for object IconCaches
        class Model: public ListModel<QString>
        {

            public:

            //! number of columns
            enum { n_columns = 1 };

            //! column type enumeration
            enum ColumnType { NAME };

            //!@name methods reimplemented from base class
            //@{

            // return data for a given index
            virtual QVariant data(const QModelIndex &index, int role) const;

            //! header data
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

            //! number of columns for a given index
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
            { return n_columns; }

            //@}

            protected:

            virtual void _sort(int, Qt::SortOrder)
            { return; }

        };

        //! update text editor selection
        void _updateSelection( const unsigned int&, const unsigned int& );

        //! replace text editor selection
        void _replaceSelection( const QString& );

        //! update suggestion list and editor for words
        void _displayWord( const QString& word );

        //! spell checking completed
        void _completed( void );

        //! model
        Model& _model( void )
        { return model_; }

        //! list
        TreeView& _list( void ) const
        { return *list_; }

        //! spell interface
        SpellInterface interface_;

        //! text editor
        QTextEdit* editor_;

        //! initial readonly state
        bool readOnlyEditor_;

        //! line editor for original word
        AnimatedLineEditor *sourceEditor_;

        //! line editor for text replacement
        AnimatedLineEditor *replaceEditor_;

        //! model
        Model model_;

        //! listbox for suggestions
        TreeView *list_;

        //! combo box for dictionary
        QComboBox *dictionary_;

        //! filter label
        QLabel *filterLabel_;

        //! combo box for filter
        QComboBox *filter_;

        //! state frame for message
        QLabel* stateLabel_;

        //! list of automatic replace words
        std::map< QString, QString > replacedWords_;

    };
};

#endif
