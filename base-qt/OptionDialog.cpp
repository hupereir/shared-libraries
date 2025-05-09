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

#include "OptionDialog.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"
#include "XmlOptions.h"


#include <QPushButton>
#include <QLabel>
#include <QLayout>

//__________________________________________________________________________
OptionDialog::OptionDialog( QWidget* parent ):
    Dialog( parent, CloseButton )
{

    Debug::Throw( QStringLiteral("OptionDialog::OptionDialog.\n") );
    setWindowTitle( tr( "Runtime Options" ) );
    setOptionName( QStringLiteral("OPTION_DIALOG") );

    QtUtil::setMargin(layout(), 0);
    QtUtil::setMargin(&buttonLayout(), defaultMargin());

    // tell dialog to delete when close
    setAttribute( Qt::WA_DeleteOnClose );

    // set model editable
    model_.setReadOnly( false );

    auto layout = new QHBoxLayout;
    layout->setSpacing(20);
    QtUtil::setMargin(layout, 5);
    mainLayout().addLayout( layout );

    //! try load Question icon
    QLabel* label = new QLabel( this );
    label->setPixmap( IconEngine::get( IconNames::DialogWarning ).pixmap( iconSize() ) );
    layout->addWidget( label );

    layout->addWidget( label = new QLabel(
        tr( "This dialog allows one to modify options manually by editting them in the list. "
        "The modifications are directly applied to the application. The reload button allows "
        "one to restore the state of the application at the time the dialog was oppened." ), this ), 1 );

    label->setWordWrap( true );

    // insert list
    mainLayout().addWidget( list_ = new TreeView( this ) );
    list_->setModel( &model_ );
    list_->setRootIsDecorated( true );

    QtUtil::setWidgetSides( list_, Qt::TopEdge|Qt::BottomEdge);

    // replace delegate
    if( list_->itemDelegate() ) list_->itemDelegate()->deleteLater();
    list_->setItemDelegate( new TextEditionDelegate( this ) );

    list_->setIconSize( QSize( 16, 16 ) );
    list_->setOptionName( QStringLiteral("OPTION_LIST") );

    // store current option state as backup and update model
    backupOptions_ = XmlOptions::get();
    _reload();

    // connections
    connect( &model_, &OptionModel::optionModified, this, &OptionDialog::_optionModified );
    connect( &model_, &OptionModel::specialOptionModified, this, &OptionDialog::_specialOptionModified );
    connect( this, &OptionDialog::configurationChanged, &Base::Singleton::get(), &Base::Singleton::requestConfigurationChanged );

    // insert reload
    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( IconNames::Reload ), QStringLiteral("&Reload"), this ) );
    connect( button, &QAbstractButton::clicked, this, &OptionDialog::_reload );
    button->setAutoDefault( false );

}

//______________________________________________________________
void OptionDialog::_reload()
{

    Debug::Throw( QStringLiteral("OptionDialog::_reload.\n") );

    model_.clear();

    // retrieve all special options
    const Options::SpecialMap specialOptions( backupOptions_.specialOptions() );
    for( auto&& iter = specialOptions.begin(); iter != specialOptions.end(); ++iter )
    {
        model_.add( Options::Pair( iter.key(), Option() ) );
        for( const auto& option:iter.value() )
        { model_.add( Options::Pair( iter.key(), option ) ); }

    }

    // retrieve normal options
    const Options::Map& options( backupOptions_.options() );
    OptionModel::List optionList;
    for( auto&& iter = options.begin(); iter != options.end(); ++iter )
    { optionList.append( Options::Pair( iter.key(), iter.value() ) ); }
    model_.add( optionList );

    list_->resizeColumns();
    list_->setColumnWidth( OptionModel::Current, IconSize::Small+4 );

    // check signal
    if( !( XmlOptions::get() == backupOptions_ ) )
    {
        XmlOptions::get() = backupOptions_;
        emit configurationChanged();
    }

}

//______________________________________________________________
void OptionDialog::_optionModified( const Options::Pair &option )
{
    Debug::Throw() << "OptionDialog::_optionModified - " << option.first << " value: " << option.second.raw() << Qt::endl;
    if( XmlOptions::get().raw( option.first ) != option.second.raw() )
    {
        XmlOptions::get().setRaw( option.first, option.second.raw() );
        emit configurationChanged();
    }

    return;

}

//______________________________________________________________
void OptionDialog::_specialOptionModified( const Options::Pair &option )
{
    Debug::Throw() << "OptionDialog::_specialOptionModified - " << option.first << Qt::endl;

    // find all matching options from model
    QModelIndex index( model_.index( Options::Pair( option.first, Option() ) ) );
    Q_ASSERT( index.isValid() );

    OptionModel::List values( model_.children( index ) );
    XmlOptions::get().clearSpecialOptions( option.first );

    if( !values.empty() )
    {

        XmlOptions::get().keep( option.first );
        for( const auto& optionPair:values )
        {
            if( !optionPair.second.raw().isEmpty() )
            { XmlOptions::get().add( optionPair.first, optionPair.second ); }
        }

    }

}
