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
  \file TreeViewConfiguration.cpp
  \brief Configuration vbox for TreeView columns
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <cassert>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QTextStream>
#include <QToolTip>

#include "Debug.h"
#include "TreeViewConfiguration.h"
#include "XmlOptions.h"

using namespace std;

//____________________________________________________________________________
TreeViewConfiguration::TreeViewConfiguration( QWidget *parent, QTreeView *target, const QString& option_name ):
    QGroupBox( parent ),
    OptionWidget( option_name )
{

  Debug::Throw( "TreeViewConfiguration::TreeViewConfiguration.\n" );

  // check target
  assert( target );

  // create vbox layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin(5);
  layout()->setSpacing( 2 );

  // size
  QCheckBox *checkbox;

  // retrieve columns
  QHeaderView* header( target->header() );
  assert( header );
  for( int index=0; index < header->count(); index++ )
  {

    // retrieve column name
    QString column_name( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
    if( column_name.isNull() || column_name.isEmpty() )
    { QTextStream( &column_name ) << "column " << index+1; }

    // add checkbox
    checkbox = new QCheckBox( column_name, this );
    layout()->addWidget( checkbox );
    checkbox_.push_back( checkbox );

    // add tooltip
    QString buffer;
    QTextStream( &buffer ) << "Show/hide column \"" << column_name << "\"";
    checkbox->setToolTip( buffer );

  }

}

//____________________________________________________________________________
void TreeViewConfiguration::read( void )
{
  Debug::Throw( "TreeViewConfiguration::read.\n" );

  // set check button state according to the backup mask
  unsigned int mask( XmlOptions::get().get<unsigned int>( optionName() ) );
  for( unsigned int index = 0; index < checkbox_.size(); index++ )
  { checkbox_[index]->setChecked( mask & (1<<index) ); }

  return;

}

//____________________________________________________________________________
void TreeViewConfiguration::write( void ) const
{
  Debug::Throw( "TreeViewConfiguration::write.\n" );

  unsigned int mask(0);
  for( unsigned int index = 0; index < checkbox_.size(); index++ )
  { mask |= (checkbox_[index]->isChecked() << index); }

  XmlOptions::get().set<unsigned int>( optionName(), mask );

  return;

}
