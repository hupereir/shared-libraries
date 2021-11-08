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

#include "Functors.h"
#include "base_qt_export.h"

#include <QTcpSocket>

class BASE_QT_EXPORT BaseSocketInterface: public QObject
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseSocketInterface( QObject*, QTcpSocket* = nullptr );

    //*@name accessors
     //@{

    //* associated socket
    const QTcpSocket& socket() const
    { return *socket_; }

    //* socket state
    QAbstractSocket::SocketState socketState() const
    { return socket_->state(); }

    //@}

    //*@name modifiers
    //@{

    //* socket
    QTcpSocket& socket()
    { return *socket_; }

    //* write buffer of a given type
    void sendBuffer( qint32, const QByteArray& );

    //@}

    //* used to retrieve all readers for a given state
    using SameStateFTor = Base::Functor::Unary<BaseSocketInterface, QAbstractSocket::SocketState, &BaseSocketInterface::socketState>;

    Q_SIGNALS:

    //* received buffer of given type
    void bufferReceived( qint32, QByteArray );

    protected:

    //* send pending buffers
    void _sendPendingBuffers();

    //* read message from socket
    void _read();

    private:

    //* send buffer
    void _sendBuffer( qint32, const QByteArray& );

    //* socket
    QTcpSocket* socket_ = nullptr;

    //* running type
    using buffertype_t = qint32;
    buffertype_t bufferType_ = -1;

    //* running lenght
    using buffersize_t = quint64;
    buffersize_t bufferSize_ = 0;

    //* buffer
    QByteArray buffer_;

    //* pending buffers
    using BufferPair = QPair<quint32, QByteArray>;
    QList<BufferPair> pendingBuffers_;

};

#endif
