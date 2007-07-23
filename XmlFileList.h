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

#include <list>     
#include <string>   
#include <qdom.h>   

#include "FileList.h"

//! Xml interface to FileList
class XmlFileList: public FileList
{

  public:

  //! constuctor
  XmlFileList( void )
  {}
  
  //! constructor
  XmlFileList( const FileList& file_list ):
    FileList( file_list )
    {}

  //! destructor
  virtual ~XmlFileList( void )
  {
    Debug::Throw( "XmlFileList::~XmlFileList.\n" );
    write();
  }

  //! set db file
  virtual void setDBFile( const File& file );
  
  //! Read fileList from file
  virtual bool read( void );

  //! write fileList to file
  virtual bool write( void );

  private:
  
  //! read flat list (for backward compatibility)
  bool _deprecatedRead( void );
  
  //! file from/to wich the files are saved
  File db_file_;

};
#endif
