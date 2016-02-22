#ifndef BaseSocketInterface_h
#define BaseSocketInterface_h

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

#include "Counter.h"

#include <QTcpSocket>

class BaseSocketInterface: public QObject, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    BaseSocketInterface( QObject*, QTcpSocket* );

    //* destructor
    virtual ~BaseSocketInterface( void ) = default;

    //* socket
    QTcpSocket& socket( void )
    { return *socket_; }

    //* associated socket
    virtual const QTcpSocket& socket( void ) const
    { return *socket_; }

    //* used to retrieve all readers for a given state
    class SameStateFTor
    {
      public:

      //* constructor
      SameStateFTor( QAbstractSocket::SocketState state ):
        state_( state )
        {}

      //* destructor
      virtual ~SameStateFTor( void )
      {}

      //* predicate
      virtual bool operator() ( const BaseSocketInterface* reader ) const
      { return reader->socket().state() == state_; }

      private:

      //* prediction
      QAbstractSocket::SocketState state_;

    };

    Q_SIGNALS:

    //* received buffer of given type
    void bufferReceived( qint32, QByteArray );

    public Q_SLOTS:

    //* write buffer of a given type
    void sendBuffer( qint32, const QByteArray& );

    protected Q_SLOTS:

    //* send pending buffers
    virtual void _sendPendingBuffers( void );

    //* read message from socket
    virtual void _read( void );

    private:

    //* send buffer
    void _sendBuffer( qint32, const QByteArray& );

    //* socket
    QTcpSocket* socket_ = nullptr;

    //* running type
    qint32 bufferType_ = -1;

    //* running lenght
    quint64 bufferSize_ = 0;

    //* buffer
    QByteArray buffer_;

    //* pending buffers
    using BufferPair = QPair<quint32, QByteArray>;
    QList<BufferPair> pendingBuffers_;

};

#endif
