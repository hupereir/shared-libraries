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

#include "Command.h"
#include "Debug.h"
#include "BaseFileInfo.h"
#include "BaseFileInfoModel.h"
#include "BaseIconNames.h"
#include "ElidedLabel.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "TreeView.h"
#include "XmlOptions.h"

#include <QDesktopServices>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QScrollBar>
#include <QUrl>

//____________________________________________________________________________
OpenWithDialog::OpenWithDialog( QWidget* parent ):
    CustomDialog( parent, OkButton|CancelButton|Separator )
{
    Debug::Throw( "OpenWithDialog::OpenWithDialog.\n" );
    okButton().setText( tr( "Open" ) );
}

//____________________________________________________________________________
void OpenWithDialog::realizeWidget( void )
{

    Q_ASSERT( !files_.isEmpty() );

    // try load Question icon
    QHBoxLayout *hLayout( new QHBoxLayout() );
    hLayout->setSpacing(10);
    hLayout->setMargin(0);
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
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->setMargin(0);
        hLayout->addLayout( vLayout );

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
        foreach( auto file, files_ )
        { fileInfoList.append( BaseFileInfo( file ) ); }

        // list of multiple files
        using FileInfoModel = BaseFileInfoModel<BaseFileInfo>;
        FileInfoModel* model = new FileInfoModel( this );
        model->setShowIcons( false );
        model->set( fileInfoList );
        model->sort( FileInfoModel::Filename, Qt::DescendingOrder );

        // file list
        TreeView* treeView = new TreeView();
        treeView->setSelectionMode( QAbstractItemView::NoSelection );
        mainLayout().addWidget( treeView );

        treeView->setModel( model );
        treeView->toggleShowHeader( false );
        treeView->setMask( 1<<FileInfoModel::Filename );
        treeView->header()->setSortIndicator( FileInfoModel::Filename, Qt::AscendingOrder );

        // resize list to accomodate longest item
        int maxWidth( 0 );
        foreach( auto fileInfo, fileInfoList )
        { maxWidth = qMax( maxWidth, treeView->fontMetrics().width( fileInfo.file() ) ); }

        treeView->verticalScrollBar()->adjustSize();
        treeView->setMinimumSize( QSize(
            maxWidth + treeView->verticalScrollBar()->width() + 10,
            treeView->fontMetrics().height() + 10 ) );

    }

    {
        // horizontal layout for combobox
        QHBoxLayout *hLayout( new QHBoxLayout() );
        hLayout->setMargin(0);
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
        foreach( auto option, applications )
        { comboBox_->addItem( File( option.raw() ) ); }
    }

    // sytem default
    comboBox_->insertItem(0, tr( "System Default" ) );
    comboBox_->setCurrentIndex( 0 );

    // connect
    connect( &okButton(), SIGNAL(clicked()), this, SLOT(_open()) );

}

//____________________________________________________________________________
void OpenWithDialog::_open( void )
{
    Debug::Throw( "OpenWithDialog::_open.\n" );

    if( comboBox_->currentIndex() == 0 )
    {

        foreach( auto file, files_ )
        {
            if( isLink_ ) QDesktopServices::openUrl( QUrl::fromEncoded( file.toLatin1() ) );
            else QDesktopServices::openUrl( QUrl::fromEncoded( QString( "file://%1" ).arg( file ).toLatin1() ) );
        }

    } else {

        // retrieve application from combobox and add as options
        const QString command = comboBox_->command();
        if( command.isEmpty() || !comboBox_->isItemValid() )
        {
            InformationDialog( this, tr( "No command specified to open the selected files. <Open> canceled." ) ).exec();
            return;
        }

        // update options
        if( !optionName_.isEmpty() && XmlOptions::get().isSpecialOption( optionName_ ) )
        {
            foreach( auto command, comboBox_->newItems() )
            { XmlOptions::get().add( optionName_, Option( command, Option::Recordable|Option::Current ) ); }
        }

        // execute
        foreach( auto file, files_ ) { ( Command( command ) << file ).run(); }

    }

}
