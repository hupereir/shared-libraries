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

    Debug::Throw( 0, "ScratchFileRemoveDialog::ScratchFileRemoveDialog.\n" );

    // horizontal layout for list and comments
    QHBoxLayout* hLayout( new QHBoxLayout() );
    hLayout->setSpacing( 5 );
    hLayout->setMargin( 0 );
    mainLayout().addLayout( hLayout, 1 );

    // file list
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setSpacing( 5 );
    vLayout->setMargin( 0 );
    hLayout->addLayout( vLayout );

    vLayout->addWidget( new QLabel( "Selected files: ", this ) );
    vLayout->addWidget( list_ = new TreeView( this ), 1 );
    _list().setModel( &model_ );
    _list().sortByColumn( FileRecordModel::FILE );
    _list().setSelectionMode( QAbstractItemView::MultiSelection );
    _list().setMask( 1<<FileRecordModel::FILE );

    model_.add( files );
    model_.sort( FileRecordModel::FILE, Qt::AscendingOrder );
    _list().resizeColumns();
    _list().selectAll();

    // vertical layout for selection buttons and comments
    vLayout = new QVBoxLayout();
    vLayout->setSpacing( 5 );
    vLayout->setMargin( 0 );
    hLayout->addLayout( vLayout );

    // work layout
    QVBoxLayout* workLayout = new QVBoxLayout();
    workLayout->setSpacing( 5 );
    workLayout->setMargin( 0 );
    vLayout->addLayout( workLayout, 1 );

    QString buffer;
    QTextStream( &buffer )
        << "Following temporary files have" << endl
        << "been created by the application." << endl
        << endl
        << "Select the ones you want to remove";

    workLayout->addWidget( new QLabel( buffer, this ) );

    // select all
    QPushButton* button;
    vLayout->addWidget( button = new QPushButton( "&Select All", this ) );
    button->setToolTip( "Select all files in list" );
    connect( button, SIGNAL( clicked() ), &_list(), SLOT( selectAll() ) );

    // deselect all
    vLayout->addWidget( clearSelectionButton_ = new QPushButton( "&Clear Selection", this ) );
    clearSelectionButton_->setToolTip( "Deselect all files in list" );
    connect( clearSelectionButton_, SIGNAL( clicked() ), _list().selectionModel(), SLOT( clear() ) );

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
