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
 * ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * software; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA   02111-1307 USA
 *
 *
 *******************************************************************************/
/*!
   \file OptionDialog.cpp
   \brief displays all run-time options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QLabel>
#include <QLayout>

#include "BaseIcons.h"
#include "OptionDialog.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "Singleton.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"
#include "Str.h"
#include "XmlOptions.h"



//__________________________________________________________________________
OptionDialog::OptionDialog( QWidget* parent ):
  CustomDialog( parent, OkButton )
{

  Debug::Throw( "OptionDialog::OptionDialog.\n" );
  setWindowTitle( "Runtime Options" );
  setOptionName( "OPTION_DIALOG" );

  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );

  // set model editable
  model_.setReadOnly( false );

  QHBoxLayout* layout = new QHBoxLayout();
  layout->setSpacing(20);
  layout->setMargin(0);
  mainLayout().addLayout( layout );

  //! try load Question icon
  QPixmap question_pixmap( PixmapEngine::get( ICONS::WARNING ) );

  // insert main vertical box
  if( !question_pixmap.isNull() )
  {
    QLabel* label = new QLabel( this );
    label->setPixmap( question_pixmap );
    layout->addWidget( label );
  }
  layout->addWidget( new QLabel(
    "This dialog allows one to modify options manually by editting them in the list.\n"
    "The modifications are directly applied to the application. The reload button allows\n"
    "one to restore the state of the application at the time the dialog was oppened.\n", this ), 1 );

  // insert list
  mainLayout().addWidget( list_ = new TreeView( this ) );
  _list().setModel( &model_ );
  _list().setRootIsDecorated( true );
  _list().setItemDelegate( new TextEditionDelegate( this ) );
  _list().setIconSize( QSize( 16, 16 ) );

  // store current option state as backup and update model
  backup_options_ = XmlOptions::get();
  _reload();

  // connections
  connect( &model_, SIGNAL( optionModified( OptionPair ) ), SLOT( _optionModified( OptionPair ) ) );
  connect( &model_, SIGNAL( specialOptionModified( OptionPair ) ), SLOT( _specialOptionModified( OptionPair ) ) );
  connect( this, SIGNAL( configurationChanged() ), Singleton::get().application(), SIGNAL( configurationChanged() ) );

  // insert OK and Cancel button
  QPushButton *button;
  buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _reload() ) );
  button->setAutoDefault( false );

}

//______________________________________________________________
void OptionDialog::_reload( void )
{

  Debug::Throw( "OptionDialog::_reload.\n" );

  // retrieve all special options
  const Options::SpecialMap special_options( backup_options_.specialOptions() );
  for( Options::SpecialMap::const_iterator iter = special_options.begin(); iter != special_options.end(); iter++ )
  {
    model_.add( OptionPair( iter->first, "" ) );
    OptionModel::List options;
    for( Options::List::const_iterator list_iter = iter->second.begin(); list_iter != iter->second.end(); list_iter++ )
    { model_.add( OptionPair( iter->first, *list_iter ) ); }

  }

  // retrieve normal options
  const Options::Map& options( backup_options_.options() );
  OptionModel::Set option_set;
  for( Options::Map::const_iterator iter = options.begin(); iter != options.end(); iter++ )
  { option_set.insert( OptionPair( iter->first, iter->second ) ); }
  model_.add( option_set );

  _list().resizeColumns();
  _list().setColumnWidth( OptionModel::CURRENT, IconSize::SMALL+4 );

  // check signal
  if( !( XmlOptions::get() == backup_options_ ) )
  {
    XmlOptions::get() = backup_options_;
    emit configurationChanged();
  }

}

//______________________________________________________________
void OptionDialog::_optionModified( OptionPair option )
{
  Debug::Throw() << "OptionDialog::_optionModified - " << option.first << " value: " << option.second.raw() << endl;
  if( XmlOptions::get().raw( option.first ) != option.second.raw() )
  {
    XmlOptions::get().setRaw( option.first, option.second.raw() );
    emit configurationChanged();
  }

  return;

}

//______________________________________________________________
void OptionDialog::_specialOptionModified( OptionPair option )
{
  Debug::Throw() << "OptionDialog::_specialOptionModified - " << option.first << endl;

  // find all matching options from model
  QModelIndex index( model_.index( OptionPair( option.first, "" ) ) );
  assert( index.isValid() );

  OptionModel::List values( model_.children( index ) );
  XmlOptions::get().clearSpecialOptions( option.first );

  if( !values.empty() )
  {

    XmlOptions::get().keep( option.first );
    for( OptionModel::List::const_iterator iter  = values.begin(); iter != values.end(); iter++ )
    {
      if( !iter->second.raw().isEmpty() )
      { XmlOptions::get().add( iter->first, iter->second ); }
    }

  }

}
