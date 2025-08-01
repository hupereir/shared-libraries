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

#include "RemoveFilesDialog.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "TreeView.h"


#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QScrollBar>

#include <numeric>

//____________________________________________________________________________
RemoveFilesDialog::RemoveFilesDialog( QWidget* parent, const FileSystemModel::List& files ):
    Dialog( parent )
{

    Debug::Throw( QStringLiteral("RemoveFilesDialog::RemoveFilesDialog.\n") );
    Q_ASSERT( !files.empty() );

    // customize layout
    QtUtil::setMargin(layout(), 0);
    layout()->setSpacing(0);
    QtUtil::setMargin(&buttonLayout(), defaultMargin());

    // options
    setOptionName( QStringLiteral("REMOVE_FILES_DIALOG") );

    // customize buttons
    okButton().setText( QStringLiteral("Delete") );
    okButton().setIcon( IconEngine::get( IconNames::Delete ) );

    // label
    QString buffer = (files.size() == 1) ?
        tr( "Permanently delete this item ?" ):
        tr( "Permanently delete these %1 items ?" ).arg( files.size() );
    auto textLabel = new QLabel( buffer, this );

    //! try load Question icon
    auto hLayout = new QHBoxLayout;
    hLayout->setSpacing(10);
    QtUtil::setMargin(hLayout, 0);
    mainLayout().addLayout( hLayout );

    auto label = new QLabel( this );
    label->setPixmap( IconEngine::get( IconNames::DialogWarning ).pixmap( iconSize() ) );
    hLayout->addWidget( label, 0, Qt::AlignHCenter );
    hLayout->addWidget( textLabel, 1, Qt::AlignLeft );

    // file list
    mainLayout().addWidget( list_ = new TreeView( this ), 1 );
    list_->setSelectionMode( QAbstractItemView::NoSelection );
    QtUtil::setWidgetSides( list_, Qt::TopEdge|Qt::BottomEdge );
    model_.setShowIcons( false );
    model_.setUseLocalNames( false );
    model_.add( files );

    // setup list
    list_->setModel( &model_ );
    list_->toggleShowHeader( false );
    list_->setMask( 1<<FileSystemModel::FileName );
    list_->setSortingEnabled( true );
    list_->header()->setSortIndicator( FileSystemModel::FileName, Qt::AscendingOrder );

    // resize list to accomodate longest item
    const auto maxWidth = std::accumulate( files.begin(), files.end(), 0,
        [this]( int maxWidth, const FileRecord& record )
        { return qMax( std::move(maxWidth), list_->fontMetrics().horizontalAdvance( record.file() ) ); } );

    list_->verticalScrollBar()->adjustSize();
    list_->setMinimumSize( QSize(
        maxWidth + list_->verticalScrollBar()->width() + 10,
        list_->fontMetrics().height() + 10 ) );


}
