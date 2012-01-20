#ifndef HelpDialog_h
#define HelpDialog_h

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

#include "AnimatedStackedWidget.h"
#include "BaseDialog.h"
#include "Counter.h"
#include "HelpModel.h"

#include <QtGui/QPushButton>
#include <cassert>

class AnimatedTextEditor;
class TreeView;

namespace BASE
{

    class HelpManager;

    //! reference manual display dialog
    class HelpDialog:public BaseDialog, public Counter
    {

        Q_OBJECT

        public:

        //! constructor
        HelpDialog( HelpManager& manager, QWidget *parent = 0 );

        //! clear items
        void clear( void )
        { model_.clear(); }

        //! set items
        void setItems( const HelpItem::List& items );

        //! adds Help item to the dialog
        void addItem( const HelpItem& item );

        //! enable/disable edition
        void setEditEnabled( const bool& value )
        { editButton_->setEnabled( value ); }

        public slots:

        //! close
        virtual void close( void )
        {

            Debug::Throw( "HelpDialog::close.\n" );
            if( model_.editionEnabled() ) _askForSave();
            QDialog::close();

        }

        protected:

        //! close event (overloaded)
        virtual void closeEvent( QCloseEvent* );

        private slots:

        //! display selected help text
        void _display( const QModelIndex&, const QModelIndex& );

        //! save modifications to current item
        void _updateItemFromEditor( QModelIndex index = QModelIndex(), bool forced = false );

        //! reload items from list and update Help manager
        void _updateHelpManager( void );

        //! toggle help edition
        void _toggleEdition( void );

        //! mode item
        void _moveItem( int row );

        //! rename item
        void _renameItem( QModelIndex index, QString value );

        //! new item
        void _newItem( void );

        //! delete current item
        void _deleteItem( void );

        //! show text
        void _showHelpString( void );

        private:

        //! if help manager is modified, ask for save
        void _askForSave( void );

        //! list
        TreeView& _list( void ) const
        { return *list_; }

        //! help manager
        HelpManager& _manager( void ) const
        {
            assert( manager_ );
            return *manager_;
        }

        //! model
        HelpModel& _model( void )
        { return model_; }

        //! help manager
        HelpManager* manager_;

        //! model
        HelpModel model_;

        //! list of help items
        TreeView *list_;

        //! stack layout to switch between editors
        AnimatedStackedWidget* stack_;

        //! html edition frame
        QWidget* htmlFrame_;

        //! plain edition frame
        QWidget* plainFrame_;

        //! read-only text editor
        AnimatedTextEditor *htmlEditor_;

        //! plain text editor
        AnimatedTextEditor *plainEditor_;

        //! edition button
        QPushButton *editButton_;

    };

};

#endif
