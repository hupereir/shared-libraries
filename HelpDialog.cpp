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

#include "AnimatedTextEditor.h"
#include "AnimatedTreeView.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "TextEditionDelegate.h"
#include "HelpManager.h"
#include "HelpModel.h"
#include "NewItemDialog.h"
#include "QuestionDialog.h"
#include "XmlOptions.h"

#include <QtGui/QAction>
#include <QtGui/QHeaderView>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QShortcut>
#include <QtCore/QTextStream>

namespace BASE
{

    //_________________________________________________________
    HelpDialog::HelpDialog( HelpManager& manager, QWidget *parent ):
        CustomDialog( parent, CloseButton ),
        manager_( &manager )
    {

        Debug::Throw( "HelpDialog::HelpDialog.\n" );


        // customize layout
        layout()->setMargin(0);
        layout()->setSpacing(0);
        buttonLayout().setMargin(5);

        // tell dialog to delete when close
        setAttribute( Qt::WA_DeleteOnClose );

        setOptionName( "HELP_DIALOG" );

        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(2);
        mainLayout().addLayout( layout );

        // add help list
        list_ = new AnimatedTreeView( this );
        list_->setMaximumWidth(150);
        layout->addWidget( list_ );
        list_->setModel( &model_ );
        list_->setItemDelegate( new TextEditionDelegate( this ) );
        list_->setItemMargin(2);
        list_->setSortingEnabled( false );
        list_->header()->hide();

        // stack widget to switch between html and plain text editor
        layout->addWidget( htmlFrame_ = new QWidget( this ) );

        // vbox layout for editor and button
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->setMargin(0);
        htmlFrame_->setLayout( vLayout );

        vLayout->addWidget( htmlEditor_ = new AnimatedTextEditor( htmlFrame_ ) );
        htmlEditor_->setReadOnly( true );
        htmlEditor_->setWrapFromOptions( false );
        htmlEditor_->wrapModeAction().setChecked( true );

        QFont font;
        font.fromString( XmlOptions::get().raw( "FONT_NAME" ) );
        htmlEditor_->setFont( font );

        // connect list to text edit
        connect( list_->selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( _display( const QModelIndex&, const QModelIndex& ) ) );

        // add close accelerator
        connect( new QShortcut( QKeySequence::Quit, this ), SIGNAL( activated() ), SLOT( close() ) );

    }

    //_________________________________________________________
    void HelpDialog::setItems( const HelpItem::List& items )
    {

        Debug::Throw( "HelpDialog::setItems.\n" );

        // clear list and editors
        htmlEditor_->clear();

        // set items
        _model().set( items );

        // select first index
        if( (!list_->selectionModel()->currentIndex().isValid()) && _model().hasIndex(0,0) )
        { list_->selectionModel()->setCurrentIndex( _model().index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows ); }


        return;

    }

    //_________________________________________________________
    void HelpDialog::_display( const QModelIndex& current, const QModelIndex& previous )
    {

        Debug::Throw( "HelpDialog::_Display.\n" );

        // check validity
        if( !current.isValid() ) htmlEditor_->clear();
        else {

            // retrieve item
            const HelpItem& item( _model().get( current ) );
            htmlEditor_->setHtml( item.text() );

        }

        return;

    }

}
