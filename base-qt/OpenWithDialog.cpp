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

#include "OpenWithDialog.h"
#include "BaseFileInfo.h"
#include "BaseFileInfoModel.h"
#include "BaseIconNames.h"
#include "Command.h"
#include "Debug.h"
#include "ElidedLabel.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "QtUtil.h"
#include "TreeView.h"
#include "XmlOptions.h"


#include <QDesktopServices>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QScrollBar>
#include <QUrl>

#include <numeric>

//____________________________________________________________________________
OpenWithDialog::OpenWithDialog( QWidget* parent ):
    Dialog( parent, OkButton|CancelButton|Separator )
{
    Debug::Throw( QStringLiteral("OpenWithDialog::OpenWithDialog.\n") );
    okButton().setText( tr( "Open" ) );
}

//____________________________________________________________________________
bool OpenWithDialog::isCommandValid() const
{ return comboBox_->isItemValid(); }

//____________________________________________________________________________
bool OpenWithDialog::isCommandDefault() const
{ return comboBox_->currentIndex() == 0; }

//____________________________________________________________________________
File OpenWithDialog::command() const
{ return comboBox_->command(); }

//____________________________________________________________________________
void OpenWithDialog::realizeWidget()
{

    Q_ASSERT( !files_.isEmpty() );

    // try load Question icon
    QHBoxLayout *hLayout( new QHBoxLayout );
    hLayout->setSpacing(10);
    QtUtil::setMargin(hLayout, 0);
    mainLayout().addLayout( hLayout );
    {
        // icon
        QLabel* label = new QLabel( this );
        QIcon icon = icon_;
        if( icon.isNull() ) icon = IconEngine::get( IconNames::DialogWarning );
        label->setPixmap( icon.pixmap( iconSize() ) );
        hLayout->addWidget( label, 0 );

    }

    {
        // vertical layout for question etc.
        QVBoxLayout* vLayout = new QVBoxLayout;
        QtUtil::setMargin(vLayout, 0);
        hLayout->addLayout( vLayout, 1 );

        vLayout->addStretch();

        if( isLink_ )
        {

            Q_ASSERT( files_.size() == 1 );

            vLayout->addWidget( new QLabel( tr( "Open link:" ), this ) );
            ElidedLabel* fileLabel = new ElidedLabel( files_.front(), this );
            fileLabel->setElideMode( Qt::ElideMiddle );

            // change font
            QFont font( fileLabel->font() );
            font.setStyle( QFont::StyleItalic );
            fileLabel->setFont( font );

            vLayout->addWidget( fileLabel );

        } else if( files_.size() > 1 ) {

            vLayout->addWidget( new QLabel( tr( "Open %1 files ? " ).arg( files_.size() ), this ) );

        } else {

            vLayout->addWidget( new QLabel( tr( "Open file \"%1\" ? " ).arg( files_.front().localName() ), this ) );

        }

        vLayout->addStretch();
    }

    if( files_.size() > 1 )
    {
        BaseFileInfo::List fileInfoList;
        std::transform( files_.begin(), files_.end(), std::back_inserter( fileInfoList ), []( const File& file ){ return BaseFileInfo( file ); } );

        // list of multiple files
        using FileInfoModel = BaseFileInfoModel<BaseFileInfo>;
        FileInfoModel* model = new FileInfoModel( this );
        model->setShowIcons( false );
        model->set( fileInfoList );
        model->sort( FileInfoModel::FileName, Qt::DescendingOrder );

        // file list
        TreeView* treeView = new TreeView;
        treeView->setSelectionMode( QAbstractItemView::NoSelection );
        mainLayout().addWidget( treeView );

        treeView->setModel( model );
        treeView->toggleShowHeader( false );
        treeView->setMask( 1<<FileInfoModel::FileName );
        treeView->header()->setSortIndicator( FileInfoModel::FileName, Qt::AscendingOrder );

        // resize list to accomodate longest item
        const auto maxWidth = std::accumulate( fileInfoList.begin(), fileInfoList.end(), 0,
            [&treeView]( int value, const BaseFileInfo& fileInfo )
            { return qMax( std::move(value), treeView->fontMetrics().horizontalAdvance( fileInfo.file() ) ); } );

        treeView->verticalScrollBar()->adjustSize();
        treeView->setMinimumSize( QSize(
            maxWidth + treeView->verticalScrollBar()->width() + 10,
            treeView->fontMetrics().height() + 10 ) );

    }

    {
        // horizontal layout for combobox
        QHBoxLayout *hLayout( new QHBoxLayout );
        QtUtil::setMargin(hLayout, 0);
        mainLayout().addLayout( hLayout );

        QLabel* label;
        hLayout->addWidget( label = new QLabel( tr( "Open With:" ), this ), 0);
        hLayout->addWidget( comboBox_ = new OpenWithComboBox( this ), 1 );
        label->setAlignment( Qt::AlignVCenter|Qt::AlignRight );
        label->setBuddy( comboBox_ );

    }

    // setup combobox
    if( !optionName_.isEmpty() && XmlOptions::get().isSpecialOption( optionName_ ) )
    {
        // retrieve applications from options
        const auto applications( XmlOptions::get().specialOptions( optionName_ ) );
        for( const auto& option:applications )
        { comboBox_->addItem( File( option.raw() ) ); }
    }

    // sytem default
    comboBox_->insertItem(0, tr( "System Default" ) );
    comboBox_->setCurrentIndex( 0 );

    // connection
    connect( &okButton(), &QAbstractButton::clicked, this, &OpenWithDialog::_open );

}

//____________________________________________________________________________
void OpenWithDialog::_open()
{
    Debug::Throw( QStringLiteral("OpenWithDialog::_open.\n") );

    // update options
    if( !optionName_.isEmpty() && XmlOptions::get().isSpecialOption( optionName_ ) )
    {
        for( const auto& command:comboBox_->newItems() )
        { XmlOptions::get().add( optionName_, Option( command, Option::Flag::Recordable|Option::Flag::Current ) ); }
    }

    // open
    if( autoOpen_ )
    {

        if( comboBox_->currentIndex() == 0 )
        {

            // open files using default application
            for( const auto& file:files_ )
            {
                if( isLink_ ) QDesktopServices::openUrl( QUrl::fromEncoded( file.get().toLatin1() ) );
                else QDesktopServices::openUrl( QUrl::fromEncoded( QStringLiteral( "file://%1" ).arg( file ).toLatin1() ) );
            }

        } else {

            // retrieve application from combobox and add as options
            const QString command = comboBox_->command();
            if( command.isEmpty() || !comboBox_->isItemValid() )
            {
                InformationDialog( this, tr( "No command specified to open the selected files. <Open> canceled." ) ).exec();
                return;
            }

            // execute
            for( const auto& file:files_ ) { ( Base::Command( command ) << file ).run(); }

        }

    }

}
