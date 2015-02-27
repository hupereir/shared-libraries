#ifndef ScratchFileMonitor_h
#define ScratchFileMonitor_h

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
#include "File.h"
#include "QOrderedSet.h"

#include <QObject>

//! used to automatically
class ScratchFileMonitor: public QObject, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    ScratchFileMonitor( QObject* parent = 0 ):
        QObject( parent ),
        Counter( "ScratchFileMonitor" )
    {}

    //! destructor
    virtual ~ScratchFileMonitor( void )
    {}

    public Q_SLOTS:

    //! add
    void add( const File& file );

    //! delete all scratch files
    void deleteScratchFiles( void );

    private:

    using FileSet = QOrderedSet<File>;
    using FileSetIterator = QOrderedSetIterator<File>;
    FileSet files_;

};

#endif
