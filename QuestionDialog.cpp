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
  \file Questioncpp
  \brief simplified question dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "BaseIcons.h"
#include "IconEngine.h"
#include "PixmapEngine.h"
#include "QuestionDialog.h"

//____________________________________________________________
QuestionDialog::QuestionDialog( QWidget* parent, QString text, BaseDialog::Centering dialog_center ):
  BaseDialog( parent ),
  Counter( "QuestionDialog" )
{

  Debug::Throw( "QtUtil::question\n" );

  // create vbox layout
  QVBoxLayout* layout=new QVBoxLayout();
  layout->setMargin(10);
  layout->setSpacing(10);
  setLayout( layout );

  //! try load Question icon
  QPixmap question_pixmap = PixmapEngine::get( ICONS::WARNING );

  // insert main vertical box
  if( question_pixmap.isNull() )
  { layout->addWidget( new QLabel( text, this ), 1, Qt::AlignHCenter ); }
  else
  {

    QHBoxLayout *h_layout( new QHBoxLayout() );
    h_layout->setSpacing(10);
    h_layout->setMargin(0);
    layout->addLayout( h_layout, 1 );

    QLabel* label = new QLabel( this );
    label->setPixmap( question_pixmap );
    h_layout->addWidget( label, 0, Qt::AlignHCenter );
    h_layout->addWidget( new QLabel( text, this ), 1, Qt::AlignHCenter );

  }

  // insert hbox layout for buttons
  QHBoxLayout *hbox_layout( new QHBoxLayout() );
  hbox_layout->setSpacing( 5 );
  layout->addLayout( hbox_layout );

  // insert OK button
  QPushButton *button;
  hbox_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_ACCEPT ), "&Yes", this ) );
  connect( button, SIGNAL( clicked() ), this, SLOT( accept() ) );

  // insert Cancel button
  hbox_layout->addWidget( button  = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&No", this ) );
  connect( button, SIGNAL( clicked() ), this, SLOT( reject() ) );

  // manage widget
  layout->activate();
  adjustSize();

  // manage widget
  switch( dialog_center ) {
    case CENTER_ON_POINTER:
      centerOnPointer();
      break;

    case CENTER_ON_PARENT:
      centerOnParent();
      break;

    case CENTER_ON_DESKTOP:
      centerOnDesktop();
      break;

    default: break;
  }

}
