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

#ifndef OpenPreviousMenu_h
#define OpenPreviousMenu_h

/*!
   \file   OpenPreviousMenu.h
   \brief   customized popup menu to open a previous logbook
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QMenu>
#include <string>
#include <list>

#include "XmlFileList.h"
#include "Debug.h"
#include "ValidFileThread.h"

/*!
   \class   OpenPreviousMenu
   \brief   customized popup menu to open a previously opened file
*/

class OpenPreviousMenu: public QMenu, public XmlFileList
{

  //! Qt meta object declaration
  Q_OBJECT

  public:

  //! constructor
  OpenPreviousMenu( QWidget *parent );

  //! destructor
  virtual ~OpenPreviousMenu( void );

  //! read db file
  virtual bool read( void );

  //! check if a file is in database, adds it if not
  virtual FileRecord& add( const std::string& file )
  { return XmlFileList::add( File( file ).expand() ); }

  //! check if a file is in database, removes it if yes
  virtual void remove( const std::string& file )
  { XmlFileList::remove( File( file ).expand() ); }

  //! check if a file is in database, adds it if not, select the file
  virtual void select( const std::string& file )
  {
    File new_file( File( file ).expand() );
    emit fileSelected( add( new_file ) );
  }

  //! open last valid file, returns true if any
  bool openLastValidFile( void );


  signals:

  //! signal emited when a file is selected
  void fileSelected( FileRecord );
  
  protected:

  //! run thread to check file validity
  void _checkValidFiles( void );
  
  //! custom event, used to retrieve file validity check event
  void customEvent( QEvent* );
  
  protected slots:
  
  //! configuration
  void _updateConfiguration( void );
  
  //! configuration
  void _saveConfiguration( void );
  

  //! remove unfound files from file list
  void _clean( void );
  //! open file at position i
  void _open( QAction* );

  //! display files from internal list
  void _loadFiles( void );

  private:
  
  //! thread to check file validity
  ValidFileThread valid_file_thread_;
  
  //! clean action
  QAction* clean_action_;
  
  //! map actions to file recors
  typedef std::map<QAction*, FileRecord> ActionMap;
  
  //! map actions to FileRecords
  ActionMap actions_;
  
};

#endif
