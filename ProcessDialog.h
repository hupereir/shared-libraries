
#ifndef _ProcessDialog_h_
#define _ProcessDialog_h_

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
  \file ProcessDialog.h
  \brief customized dialog to handle QProcess objects and throw output in the text
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QDialog>

#include "ProcessFrame.h"

/*!
  \class ProcessDialog
  \brief customized dialog to handle QProcess objects and throw output in the text
*/

class ProcessDialog: public QDialog, public Counter
{

  public:
  
  //! creator
  ProcessDialog( QWidget* parent );
  
  //! destructor
  virtual ~ProcessDialog( void )
  { Debug::Throw( "ProcessDialog::~ProcessDialog.\n" ); }
  
  //! process frame
  ProcessFrame& processFrame( void )
  {
    Exception::checkPointer( process_frame_, DESCRIPTION( "process_frame_ not initialized" ) );
    return *process_frame_;
  }

  private:
  
  //! associated process
  ProcessFrame *process_frame_;


};

#endif

