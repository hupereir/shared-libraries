// $Id$
#ifndef _CustomFileDialog_h_
#define _CustomFileDialog_h_
 
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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/
 
/*!
  \file CustomFileDialog.h
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <qfiledialog.h>


#include "Counter.h"  
#include "File.h"  

/*!
  \class CustomFileDialog
  \brief customized file selection dialog, to store last selected directory
*/
class CustomFileDialog: public QFileDialog, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! creator
  CustomFileDialog( QWidget* parent );  

  //! working directory
  static const QString& workingDirectory( void )
  { return working_directory_; }  

  private slots:
  
  //! stores working directory when changed
  void _saveWorkingDirectory( const QString& directory );
    
  private:
  
  //! static string to store working directory
  static QString working_directory_;
  
};

#endif
