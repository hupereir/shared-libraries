#ifndef XmlFileList_h
#define XmlFileList_h

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
\file XmlFileList.h
\brief Xml interface to FileList
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QString>
#include <qdom.h>

#include "FileList.h"

//! Xml interface to FileList
class XmlFileList: public FileList
{

    //! Qt meta object declaration
    Q_OBJECT

        public:

        //! constuctor
        XmlFileList( QObject* parent = 0 );

    //! destructor
    virtual ~XmlFileList( void )
    {}

    protected:

    //! Read fileList from file
    virtual bool _read( void );

    //! write fileList to file
    virtual bool _write( void );

    //! set db file
    virtual bool _setDBFile( const QString& file );

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    //! save configuration
    void _saveConfiguration( void );

    private:

    //! read flat list (for backward compatibility)
    bool _deprecatedRead( void );

    //! file from/to wich the files are saved
    QString dbFile_;

};
#endif
