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

#include "ScratchFileRemoveDialog.h"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QtGui/QLabel>

//____________________________________________________________________________
ScratchFileRemoveDialog::ScratchFileRemoveDialog( QWidget* parent, const FileRecordModel::List& files ):
CustomDialog( parent )
{

    Debug::Throw( "ScratchFileRemoveDialog::ScratchFileRemoveDialog.\n" );

    setOptionName( "SCRATCH_DIALOG" );

    // label
    QLabel* label;
    mainLayout().addWidget( label = new QLabel(
        "Following temporary files have been created by the application. "
        "Select the ones you want to remove.\n", this ) );

    // file list
    mainLayout().addWidget( list_ = new TreeView( this ), 1 );
    _list().setModel( &model_ );
    _list().sortByColumn( FileRecordModel::FILE );
    _list().setSelectionMode( QAbstractItemView::MultiSelection );
    _list().setMask(
        (1<<FileRecordModel::FILE)|
        (1<<FileRecordModel::PATH) );

    model_.add( files );
    model_.sort( FileRecordModel::FILE, Qt::AscendingOrder );
    _list().resizeColumns();
    _list().selectAll();

    // deselect all
    buttonLayout().insertWidget( 0, clearSelectionButton_ = new QPushButton( "&Clear Selection", this ) );
    clearSelectionButton_->setToolTip( "Deselect all files in list" );
    connect( clearSelectionButton_, SIGNAL( clicked() ), _list().selectionModel(), SLOT( clear() ) );

    // select all
    QPushButton* button;
    buttonLayout().insertWidget( 0, button = new QPushButton( "&Select All", this ) );
    button->setToolTip( "Select all files in list" );
    connect( button, SIGNAL( clicked() ), &_list(), SLOT( selectAll() ) );

    // connection
    connect( _list().selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ), SLOT( _updateButtons() ) );
    _updateButtons();

    // customize dialog buttons
    okButton().setText( "&Remove" );
    okButton().setToolTip( "Remove selected files from disk" );
    okButton().setFocus();

    cancelButton().setText( "&Ignore" );
    cancelButton().setToolTip( "Leave files on disk and proceed" );

}

//____________________________________________________________________________
FileRecordModel::List ScratchFileRemoveDialog::selectedFiles( void ) const
{ return _model().get( _list().selectionModel()->selectedRows() ); }

//____________________________________________________________________
void ScratchFileRemoveDialog::_updateButtons( void )
{
    Debug::Throw( "ScratchFileRemoveDialog::_updateButtons.\n" );
    bool has_selection( !_list().selectionModel()->selectedRows().empty() );
    clearSelectionButton_->setEnabled( has_selection );
    okButton().setEnabled( has_selection );
}
