#ifndef BaseFileIconProvider_h
#define BaseFileIconProvider_h

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "BaseFileInfo.h"
#include "Counter.h"
#include "File.h"

#include <QMap>
#include <QObject>
#include <QIcon>

//! returns icon for a given FileInfo
class BaseFileIconProvider: public QObject, public Counter
{

    public:

    //! constructor
    BaseFileIconProvider( QObject* = 0x0 );

    //! destructor
    virtual ~BaseFileIconProvider( void )
    {}

    //! icon matching given file info
    virtual const QIcon& icon( const BaseFileInfo& fileInfo )
    { return icon( fileInfo, fileInfo.type() ); }

    //! icon matching given file info
    virtual const QIcon& icon( const BaseFileInfo&, int );

    private:

    class Icon
    {

        public:

        //! constructor
        Icon( const QString& name ):
            name_( name )
        {}

        //! icon
        const QIcon& icon( bool );

        private:

        //! icon name
        QString name_;

        //! icon
        QIcon icon_;

        //! link icon
        QIcon linkIcon_;

    };


    typedef QMap<File, Icon> IconMap;
    IconMap icons_;

};

#endif
