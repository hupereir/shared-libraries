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

#include "HelpDialog.h"
#include "HelpManager.h"
#include "HelpModel.h"
#include "QtUtil.h"
#include "QuestionDialog.h"
#include "Singleton.h"
#include "TextEditionDelegate.h"
#include "TextEditor.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QAction>
#include <QDesktopServices>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QToolButton>
#include <QShortcut>
#include <QTextStream>
#include <QUrl>

namespace Base
{

    //_________________________________________________________
    HelpDialog::HelpDialog( QWidget *parent ):
        Dialog( parent, CloseButton )
    {

        Debug::Throw( QStringLiteral("HelpDialog::HelpDialog.\n") );


        // customize layout
        QtUtil::setMargin(layout(), 0);
        layout()->setSpacing(0);
        QtUtil::setMargin(&buttonLayout(), defaultMargin());

        setOptionName( QStringLiteral("HELP_DIALOG") );

        auto layout = new QHBoxLayout;
        QtUtil::setMargin(layout, 0);
        layout->setSpacing(2);
        mainLayout().addLayout( layout );

        // add help list
        list_ = new TreeView( this );
        QtUtil::setWidgetSides( list_, Qt::TopEdge|Qt::BottomEdge );

        list_->setMaximumWidth(150);
        layout->addWidget( list_ );
        list_->setModel( &model_ );

        list_->setSortingEnabled( false );
        list_->header()->hide();

        // stack widget to switch between html and plain text editor
        layout->addWidget( htmlFrame_ = new QWidget( this ) );

        // vbox layout for editor and button
        auto vLayout = new QVBoxLayout;
        QtUtil::setMargin(vLayout, 0);
        htmlFrame_->setLayout( vLayout );

        vLayout->addWidget( htmlEditor_ = new TextEditor( htmlFrame_ ) );
        QtUtil::setWidgetSides( htmlEditor_, Qt::TopEdge|Qt::BottomEdge );
        htmlEditor_->setReadOnly( true );
        htmlEditor_->setWrapFromOptions( false );
        htmlEditor_->wrapModeAction().setChecked( true );
        htmlEditor_->setTrackAnchors( true );
        connect( htmlEditor_, &TextEditor::linkActivated, this, &HelpDialog::_openLink );

        // connect list to text edit
        connect( list_->selectionModel(), &QItemSelectionModel::currentChanged, this, &HelpDialog::_display );

        // add close accelerator
        connect( new QShortcut( QKeySequence::Quit, this ), &QShortcut::activated, this, &HelpDialog::close );

        connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &HelpDialog::_updateConfiguration );
        _updateConfiguration();

    }

    //_________________________________________________________
    void HelpDialog::setItems( const HelpItem::List& items )
    {

        Debug::Throw( QStringLiteral("HelpDialog::setItems.\n") );

        // clear list and editors
        htmlEditor_->clear();

        // set items
        model_.set( items );

        // select first index
        if( (!list_->selectionModel()->currentIndex().isValid()) && model_.hasIndex(0,0) )
        { list_->selectionModel()->setCurrentIndex( model_.index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows ); }


        return;

    }

    //_________________________________________________________
    void HelpDialog::_display( const QModelIndex& current, const QModelIndex& )
    {

        Debug::Throw( QStringLiteral("HelpDialog::_Display.\n") );

        // check validity
        if( !current.isValid() ) htmlEditor_->clear();
        else {

            // retrieve item
            const auto item( model_.get( current ) );
            htmlEditor_->setHtml( item.text() );

        }

        return;

    }

    //_________________________________________________________
    void HelpDialog::_updateConfiguration()
    {
        Debug::Throw( QStringLiteral("HelpDialog::_updateConfiguration.\n") );
        htmlEditor_->setFont( qApp->font() );
    }

    //_________________________________________________________
    void HelpDialog::_openLink( const QString &link )
    {
        Debug::Throw() << "HelpDialog::_openLink - link: " << link << Qt::endl;
        QDesktopServices::openUrl( QUrl(link) );
    }
}
