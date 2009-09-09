#ifndef _MessageBuffer_h_
#define _MessageBuffer_h_

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
  \file MessageBuffer.h
  \brief customized QVBox to recieve close event
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/


#include <QString>

//! buffer class and current position
class MessageBuffer
{
  public:

  //! constructor
  MessageBuffer( void ):
    position_( 0 )
    {}

  //! clear
  void clear( void )
  {
    text_ = QString();
    position_ = 0;
  }

  //! append
  void append( const QString& buffer )
  { text_ += buffer; }

  //! move to position
  void flush( void )
  { flush( position() ); }

  //! move to new position
  void flush( const int& position )
  {
    text_ = text_.mid( position );
    position_ = 0;
  }

  //! text
  const QString& text( void ) const
  { return text_; }

  //! text
  QString& text( void )
  { return text_; }

  //! position
  const int& position( void ) const
  { return position_; }

  private:

  //! text
  QString text_;

  //! last parsed position
  int position_;

};
#endif
