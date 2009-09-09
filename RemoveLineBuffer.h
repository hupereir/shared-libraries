#ifndef RemoveLineBuffer_h
#define RemoveLineBuffer_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file RemoveLineBuffer.h
  \brief handles consecutive calls to remove line in a single Clipboard block
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QString>
#include <QObject>

#include "Counter.h"
#include "Debug.h"

//! handles consecutive calls to remove line in a single Clipboard block
class RemoveLineBuffer: public QObject, public Counter
{

  Q_OBJECT

  public:

  //! constructor
  RemoveLineBuffer( QObject* parent ):
    QObject( parent ),
    Counter( "RemoveLineBuffer" )
  { Debug::Throw( "RemoveLineBuffer::RemoveLineBuffer.\n" ); }

  //! append string to buffer
  void append( const QString& value );

  public slots:

  //! clear buffer to clipboard
  void clear( void );

  private:

  //! buffer string
  QString buffer_;

};

#endif
