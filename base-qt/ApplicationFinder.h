#ifndef ApplicationFinder_h
#define ApplicationFinder_h

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

#include <QObject>

class LineEditor;

class ApplicationFinder: public QObject, public Counter
{
    Q_OBJECT

    public:

    //* constructor
    ApplicationFinder( QObject* );

    //* add a file to be found
    void add( File, LineEditor* );

    public Q_SLOTS:

    //* update all applications
    void update( void );

    private:

    using Pair = QPair<File, LineEditor*>;
    using PairList = QList<Pair>;

    PairList applications_;

};

#endif
