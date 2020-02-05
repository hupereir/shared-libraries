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

#include "DetailsDialog.h"
#include "TextEditor.h"

//____________________________________________________________
void DetailsDialog::setDetails( const QString& details )
{

    Debug::Throw( QStringLiteral("DetailsDialog::setDetails.\n") );

    // do nothing if detail string is invalid
    if( details.isEmpty() ) return;

    // create text editor assign text and set as details
    auto editor = new TextEditor;
    editor->setReadOnly( true );
    editor->setWrapFromOptions( false );
    editor->setLineWrapMode( TextEditor::WidgetWidth );
    editor->setText( details );
    setDetails( editor );

}

//____________________________________________________________
void DetailsDialog::setDetails( QWidget* widget )
{

    Debug::Throw( QStringLiteral("DetailsDialog::setDetails.\n") );

    // details label
    if( !detailsLabel_ )
    {
        detailsLabel_ = new QLabel( tr( "<a href='details'>details...</a>" ), this );
        detailsLabel_->setAlignment( Qt::AlignRight|Qt::AlignVCenter );
        mainLayout().addWidget( detailsLabel_ );

        connect( detailsLabel_, &QLabel::linkActivated, this, &DetailsDialog::showDetails );
    }

    // assign widget
    detailsWidget_.reset( widget );
    if( detailsWidget_->parentWidget() != this ) detailsWidget_->setParent( this );
    mainLayout().addWidget( detailsWidget_.get(), 1 );
    detailsWidget_->hide();

}

//_____________________________________________________________________
void DetailsDialog::toggleDetails( bool visible )
{
    Debug::Throw( QStringLiteral("toggleDetails::showDetails.\n") );
    if( !detailsWidget_ ) return;

    detailsLabel_->setVisible( !visible );
    detailsWidget_->setVisible( visible );

    // also hide separator
    if( hasSeparator() ) separator().setVisible( !visible );
    adjustSize();

}
