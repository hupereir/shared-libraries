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

/*!
  \file QuestionDialog.cpp
  \brief simplified question dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPushButton>

#include "BaseIcons.h"
#include "IconEngine.h"
#include "PixmapEngine.h"
#include "QuestionDialog.h"

//____________________________________________________________
QuestionDialog::QuestionDialog( QWidget* parent, QString text ):
  CustomDialog( parent, OK_BUTTON | CANCEL_BUTTON )
{

  Debug::Throw( "QtUtil::question\n" );

  text_layout_ = new QVBoxLayout();
  text_layout_->setSpacing(5);
  text_layout_->setMargin(0);
  text_layout_->addWidget( label_ = new QLabel( text, this ) ); 
  
  //! try load Question icon
  QPixmap question_pixmap = PixmapEngine::get( ICONS::WARNING );

  if( question_pixmap.isNull() ) mainLayout().addItem( text_layout_ );
  else {

    QHBoxLayout *h_layout( new QHBoxLayout() );
    h_layout->setSpacing(10);
    h_layout->setMargin(0);
    mainLayout().addLayout( h_layout );

    QLabel* label = new QLabel( this );
    label->setPixmap( question_pixmap );
    h_layout->addWidget( label, 0, Qt::AlignHCenter|Qt::AlignTop );
    h_layout->addItem( text_layout_ );

  }

}
