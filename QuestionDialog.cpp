// $Id$


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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "QuestionDialog.h"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "PixmapEngine.h"

#include <QtGui/QPushButton>

//____________________________________________________________
QuestionDialog::QuestionDialog( QWidget* parent, QString text ):
    CustomDialog( parent, OkButton | CancelButton| Separator )
{

    Debug::Throw( "QuestionDialog::QuestionDialog\n" );

    //! try load Question icon
    QPixmap pixmap = PixmapEngine::get( ICONS::WARNING );
    QHBoxLayout *hLayout( new QHBoxLayout() );
    hLayout->setSpacing(10);
    hLayout->setMargin(0);
    mainLayout().addLayout( hLayout );

    QLabel* label = new QLabel( this );
    label->setPixmap( pixmap );
    hLayout->addWidget( label, 0 );
    hLayout->addWidget( label_ = new QLabel( text, this ), 1 );
    label_->setWordWrap( true );

}
