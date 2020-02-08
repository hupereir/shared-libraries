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

#include "ScratchFileRemoveDialog.h"

#include "BaseContextMenu.h"
#include "BaseIconNames.h"
#include "ColumnSelectionMenu.h"
#include "ColumnSortingMenu.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QLabel>
#include <QHeaderView>

//____________________________________________________________________________
ScratchFileRemoveDialog::ScratchFileRemoveDialog( QWidget* parent, const FileRecordModel::List& files ):
CustomDialog( parent )
{

    Debug::Throw( QStringLiteral("ScratchFileRemoveDialog::ScratchFileRemoveDialog.\n") );

    setOptionName( QStringLiteral("SCRATCH_DIALOG") );

    // label
    QLabel* textLabel = new QLabel(
        tr( "Following temporary files have been created by the application. Select the ones you want to remove:"),
        this );

    textLabel->setWordWrap( true );

    //! try load Question icon
    QHBoxLayout *hLayout( new QHBoxLayout );
    hLayout->setSpacing(10);
    hLayout->setMargin(0);
    mainLayout().addLayout( hLayout );
    QLabel* label = new QLabel( this );
    label->setPixmap( IconEngine::get( IconNames::DialogWarning ).pixmap( iconSize() ) );
    hLayout->addWidget( label, 0, Qt::AlignHCenter );
    hLayout->addWidget( textLabel, 1, Qt::AlignLeft );

    // file list
    mainLayout().addWidget( list_ = new TreeView( this ), 1 );
    list_->setModel( &model_ );
    list_->sortByColumn( FileRecordModel::FileName, Qt::DescendingOrder );
    list_->setSelectionMode( QAbstractItemView::MultiSelection );
    list_->setContextMenuPolicy( Qt::CustomContextMenu );
    list_->setMask(
        (1<<FileRecordModel::FileName)|
        (1<<FileRecordModel::Path) );
    list_->showHeaderAction().setChecked( false );
    list_->setOptionName( QStringLiteral("SCRATCH_FILE_LIST") );

    model_.add( files );
    model_.sort( FileRecordModel::FileName, Qt::DescendingOrder );
    list_->resizeColumns();
    list_->selectAll();

    // actions
    _installActions();

    // connections
    connect( list_, &QWidget::customContextMenuRequested, this, &ScratchFileRemoveDialog::_updateContextMenu );
    connect( list_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ScratchFileRemoveDialog::_updateActions );
    _updateActions();

    // customize dialog buttons
    okButton().setText( tr( "Remove" ) );
    okButton().setIcon( IconEngine::get( IconNames::Delete ) );
    okButton().setToolTip( tr( "Remove selected files from disk" ) );
    okButton().setFocus();

    cancelButton().setText( tr( "Ignore" ) );
    cancelButton().setToolTip( tr( "Leave files on disk and proceed" ) );

}

//____________________________________________________________________________
FileRecordModel::List ScratchFileRemoveDialog::selectedFiles() const
{ return model_.get( list_->selectionModel()->selectedRows() ); }

//____________________________________________________________________
void ScratchFileRemoveDialog::_updateActions()
{
    Debug::Throw( QStringLiteral("ScratchFileRemoveDialog::_updateActions.\n") );
    bool hasSelection( !list_->selectionModel()->selectedRows().empty() );
    clearSelectionAction_->setEnabled( hasSelection );
    okButton().setEnabled( hasSelection );
}

//____________________________________________________________________
void ScratchFileRemoveDialog::_updateContextMenu( const QPoint& position )
{

    Debug::Throw( QStringLiteral("ScratchFileRemoveDialog::_updateContextMenu.\n") );

    BaseContextMenu menu( list_ );
    menu.setHideDisabledActions( true );

    menu.addAction( &list_->showHeaderAction() );
    if( !list_->showHeaderAction().isChecked() )
    {
        menu.addMenu( new ColumnSelectionMenu( &menu, list_ ) );
        menu.addMenu( new ColumnSortingMenu( &menu, list_ ) );
    }

    // new folder
    menu.addSeparator();
    menu.addAction( selectAllAction_ );
    menu.addAction( clearSelectionAction_ );

    // execute
    menu.exec( list_->viewport()->mapToGlobal( position ) );

}

//____________________________________________________________________
void ScratchFileRemoveDialog::_installActions()
{
    // deselect all
    addAction( clearSelectionAction_ = new QAction( tr( "Clear Selection" ), this ) );
    connect( clearSelectionAction_, &QAction::triggered, list_->selectionModel(), &QItemSelectionModel::clear );

    // select all
    addAction( selectAllAction_ = new QAction( tr( "Select All" ), this ) );
    connect( selectAllAction_, &QAction::triggered, list_, &TreeView::selectAll );

}
