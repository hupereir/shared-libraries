#ifndef SshTunnelAttributes_h
#define SshTunnelAttributes_h

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
#include "base_ssh_export.h"

#include <QSet>
#include <QString>
#include <QVector>

namespace Ssh
{

    //_________________________________________________________
    class BASE_SSH_EXPORT TunnelAttributes: private Base::Counter<TunnelAttributes>
    {
        public:

        using Set = QSet<TunnelAttributes>;
        using List = QVector<TunnelAttributes>;

        //* constructor
        explicit TunnelAttributes():
            Counter( QStringLiteral("Ssh::TunnelAttributes") )
        {}

        //* destructor
        virtual ~TunnelAttributes() = default;

        //*@name accessors
        //@{

        //* host
        const QString& host() const
        { return host_; }

        //* port
        int localPort() const
        { return localPort_; }

        //* port
        int remotePort() const
        { return remotePort_; }

        //* validity
        bool isValid() const
        { return !(host_.isEmpty() || localPort_ <= 0 || remotePort_ <= 0 ); }

        //@}

        //*@name modifiers
        //@{

        //* host
        void setHost( const QString& host )
        { host_ = host; }

        //* port
        void setLocalPort( int port )
        { localPort_ = port; }

        //* port
        void setRemotePort( int port )
        { remotePort_ = port; }

        //@}

        private:

        QString host_;
        int localPort_ = 0;
        int remotePort_ = 0;

        //* equal to operator
        friend bool operator == (const TunnelAttributes& first, const TunnelAttributes& second)
        { return first.localPort_ == second.localPort_; }


    };

    //____________________________________________________
    inline uint qHash( const Ssh::TunnelAttributes& attributes )
    { return attributes.localPort(); }

}

#endif
