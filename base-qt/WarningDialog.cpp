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

#include "WarningDialog.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "QtUtil.h"


#include <QLayout>

//____________________________________________________________
WarningDialog::WarningDialog( QWidget* parent, const QString& text ):
    DetailsDialog( parent, OkButton| Separator )
{

    Debug::Throw( QStringLiteral("WarningDialog::WarningDialog\n") );

    //! try load Question icon
    QHBoxLayout *hLayout( new QHBoxLayout );
    hLayout->setSpacing(10);
    QtUtil::setMargin(hLayout, 0);
    mainLayout().addLayout( hLayout );

    QLabel* label = new QLabel( this );
    label->setPixmap( IconEngine::get( IconNames::DialogWarning ).pixmap( iconSize() ) );
    hLayout->addWidget( label, 0 );
    hLayout->addWidget( label_ = new QLabel( this ), 1 );
    label_->setTextInteractionFlags( Qt::TextSelectableByMouse );
    setText( text );

}

//____________________________________________________________
void WarningDialog::setText( const QString& text )
{
    Debug::Throw( QStringLiteral("WarningDialog::setText\n") );
    label_->setText( text );
    label_->setWordWrap( label_->fontMetrics().horizontalAdvance( text ) >300 );
}
