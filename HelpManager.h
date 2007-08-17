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

#ifndef Help_h
#define Help_h

/*!
   \file    HelpManager.h
   \brief   reference manual help system
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QObject>

#include "Counter.h"
#include "CustomTextEdit.h"
#include "Debug.h"
#include "File.h"
#include "HelpDialog.h"

//! help manager class
namespace BASE
{
    
  class HelpManager:public QObject, public Counter
  {
    
    Q_OBJECT
      
    public:
      
    //! manager singleton
    static HelpManager& get( void )
    { return singleton_; }
    
    //! destructor
    virtual ~HelpManager( void )
    {
      Debug::Throw( "HelpManager::~HelpManager.\n" );
      if( modified() ) _save();
    }
    
    //! input file
    void setFile( const File& file )
    { file_ = file; }
    
    //! install help. Put items in the menu, create associated objects 
    bool install( const char *text[] );
    
    //! install help, from file
    bool install( const File& file );
    
    //! true when items have been edited/modified
    const bool& modified( void ) const
    { return dialog_->modified(); }
    
    //! set dialog caption
    void setWindowTitle( const std::string& value )
    { dialog_->setWindowTitle( value.c_str() ); }
    
    //! display action
    QAction& displayAction( void )
    { return *display_action_; }
    
    //! dump action
    QAction& dumpAction( void )
    { return *dump_action_; }
   
    protected slots:
    
    //! opens help dialog
    void _display( void );
    
    //! dump help string in a TextEditor
    void _dumpHelpString( void );
    
    //! save help to file
    void _save( void );
    
    private:
    
    //! constructor
    HelpManager( void );
    
    //! singleton
    static HelpManager singleton_;
    
    //!@name actions
    //@{
    
    //! dialog display action
    QAction* display_action_;
    
    //! dump help string
    QAction* dump_action_;
    
    //@}
    
    //! associated dialog
    HelpDialog* dialog_;
    
    //! help filename
    File file_;
    
  };
  
};

#endif
  
