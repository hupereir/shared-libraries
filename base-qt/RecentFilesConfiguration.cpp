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

#include "BaseIconNames.h"
#include "ContextMenu.h"
#include "Debug.h"
#include "FileList.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "OptionCheckBox.h"
#include "OptionSpinBox.h"
#include "QtUtil.h"
#include "QuestionDialog.h"
#include "RecentFilesConfiguration.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

//___________________________________________
RecentFilesConfiguration::RecentFilesConfiguration( QWidget* parent, FileList& recentFiles ):
    QWidget( parent ),
    OptionWidgetList( this ),
    Counter( QStringLiteral("RecentFilesConfiguration") ),
    recentFiles_( &recentFiles )
{
    Debug::Throw( QStringLiteral("RecentFilesConfiguration::RecentFilesConfiguration.\n") );

    QVBoxLayout* layout = new QVBoxLayout;
    setLayout( layout );
    QtUtil::setMargin(layout, 0);
    layout->setSpacing( 5 );

    QGroupBox* box;
    // group box for additional options
    layout->addWidget( box = new QGroupBox( tr( "Options" ), this ) );

    QVBoxLayout* vLayout = new QVBoxLayout;
    QtUtil::setMargin(vLayout, 5);
    vLayout->setSpacing(6);
    box->setLayout( vLayout );

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->setSpacing(6);
    QtUtil::setMargin(hLayout, 0);
    vLayout->addLayout( hLayout );

    // previous file history size
    OptionSpinBox *spinbox;
    hLayout->addWidget( new QLabel( tr( "Recent files history size" ), box ) );
    hLayout->addWidget( spinbox = new OptionSpinBox( box, QStringLiteral("DB_SIZE") ) );
    hLayout->addStretch(1);
    spinbox->setSpecialValueText( tr( "Unlimited" ) );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 100 );
    addOptionWidget( spinbox );
    spinbox->setToolTip( tr( "Number of previously opened files to appear in the Open Previous menu" ) );

    // sort previous files by date
    OptionCheckBox* checkbox;
    vLayout->addWidget( checkbox = new OptionCheckBox( tr( "Sort recent files by date" ), box, QStringLiteral("SORT_FILES_BY_DATE") ) );
    checkbox->setToolTip( tr( "Sort files by date rather than name in Open Previous menu." ) );
    addOptionWidget( checkbox );

    // group box for file list
    layout->addWidget( box = new QGroupBox( tr( "Files" ), this ), 1 );
    box->setLayout( new QHBoxLayout );
    box->layout()->setSpacing(6);
    box->QtUtil::setMargin(layout(), 6);

    box->layout()->addWidget( list_ = new TreeView( box ) );
    list_->setModel( &model_ );
    list_->sortByColumn( XmlOptions::get().get<bool>( QStringLiteral("SORT_FILES_BY_DATE") ) ? FileRecordModel::Time : FileRecordModel::FileName, Qt::DescendingOrder );
    list_->setSelectionMode( QAbstractItemView::ContiguousSelection );
    list_->setOptionName( QStringLiteral("RECENT_FILES_CONFIGURATION_LIST") );

    connect( list_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &RecentFilesConfiguration::_updateButtons );

    vLayout = new QVBoxLayout;
    vLayout->setSpacing(6);
    QtUtil::setMargin(vLayout, 0);
    box->layout()->addItem( vLayout );

    // list context menu
    QMenu* menu( new ContextMenu( list_ ) );

    // clean
    vLayout->addWidget( cleanButton_ = new QPushButton( tr( "Clean" ), box ) );
    cleanButton_->setToolTip( tr( "Remove invalid files" ) );
    cleanButton_->setIcon( IconEngine::get( IconNames::Delete ) );
    connect( cleanButton_, &QAbstractButton::clicked, this, &RecentFilesConfiguration::_clean );

    addAction( cleanAction_ = new QAction( IconEngine::get( IconNames::Delete ), tr( "Clean" ), this ) );
    connect( cleanAction_, &QAction::triggered, this, &RecentFilesConfiguration::_clean );
    menu->addAction( cleanAction_ );

    // remove
    vLayout->addWidget( removeButton_ = new QPushButton( tr( "Remove" ), box ) );
    removeButton_->setIcon( IconEngine::get( IconNames::Remove ) );
    removeButton_->setToolTip( tr( "Remove selected files" ) );
    connect( removeButton_, &QAbstractButton::clicked, this, &RecentFilesConfiguration::_remove );

    addAction( removeAction_ = new QAction( IconEngine::get( IconNames::Remove ), tr( "Remove" ), this ) );
    connect( removeAction_, &QAction::triggered, this, &RecentFilesConfiguration::_remove );
    removeAction_->setShortcut( QKeySequence::Delete );
    menu->addAction( removeAction_ );
    list_->addAction( removeAction_ );

    // reload
    vLayout->addWidget( reloadButton_ = new QPushButton( tr( "Reload" ), box ) );
    reloadButton_->setToolTip( tr( "Reload file list" ) );
    reloadButton_->setIcon( IconEngine::get( IconNames::Reload ) );
    connect( reloadButton_, &QAbstractButton::clicked, this, &RecentFilesConfiguration::reload );

    addAction( reloadAction_ = new QAction( IconEngine::get( IconNames::Reload ), tr( "Reload" ), this ) );
    connect( reloadAction_, &QAction::triggered, this, &RecentFilesConfiguration::reload );
    menu->addAction( reloadAction_ );

    vLayout->addStretch( 1 );

    // connections
    reloadButton_->setEnabled( false );

}

//__________________________________________________________________________
void RecentFilesConfiguration::read()
{
    Debug::Throw( QStringLiteral("RecentFilesConfiguration::read.\n") );
    reload();
    list_->resizeColumns();
}

//__________________________________________________________________________
void RecentFilesConfiguration::write()
{
    Debug::Throw( QStringLiteral("RecentFilesConfiguration::write.\n") );
    recentFiles_->set( model_.get() );
}

//__________________________________________________________________________
void RecentFilesConfiguration::reload()
{

    Debug::Throw( QStringLiteral("RecentFilesConfiguration::reload.\n") );
    const auto recordModelList( recentFiles_->records() );
    model_.set( recordModelList );

    list_->selectionModel()->clear();

    reloadButton_->setEnabled( false );
    cleanButton_->setEnabled( std::any_of( recordModelList.begin(), recordModelList.end(), FileRecord::InvalidFTor() ) );
    _updateButtons();

}

//__________________________________________________________________________
void RecentFilesConfiguration::_updateButtons()
{

    Debug::Throw( QStringLiteral("RecentFilesConfiguration::_updateButtons.\n") );
    QList<QModelIndex> selection( list_->selectionModel()->selectedRows() );
    removeButton_->setEnabled( !selection.empty() );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_remove()
{

    Debug::Throw( QStringLiteral("RecentFilesConfiguration::_remove.\n") );
    QList<QModelIndex> selection( list_->selectionModel()->selectedRows() );
    if( selection.isEmpty() ) return;

    // ask confirmation
    if( !QuestionDialog( this, tr( "Remove selected files from list ?" ) ).centerOnParent().exec() ) return;

    model_.remove( model_.get( selection ) );
    list_->selectionModel()->clear();

    reloadButton_->setEnabled( true );

    // records
    FileRecordModel::List records( model_.get() );
    cleanButton_->setEnabled( std::any_of( records.begin(), records.end(), FileRecord::InvalidFTor() ) );

}

//__________________________________________________________________________
void RecentFilesConfiguration::_clean()
{
    Debug::Throw( QStringLiteral("RecentFilesConfiguration::_clean.\n") );

    // remove invalid files
    FileRecordModel::List records( model_.get() );
    records.erase(
        std::remove_if( records.begin(), records.end(), FileRecord::InvalidFTor() ),
        records.end() );

    model_.set( records );

    reloadButton_->setEnabled( true );
    cleanButton_->setEnabled( false );
    _updateButtons();

}
