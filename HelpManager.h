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
#include "HelpItem.h"

//! help manager class
namespace BASE
{
    
  class HelpManager:public QObject, public Counter
  {
    
    Q_OBJECT
      
    public:

    //! constructor
    HelpManager( QObject* parent );
      
    //! destructor
    virtual ~HelpManager( void )
    { Debug::Throw( "HelpManager::~HelpManager.\n" ); }
    
    //!@name static accessors
    //@{
    
    //! input file
    static void setFile( const File& file )
    { file_ = file; }
       
    //! list of help items
    typedef std::vector< HelpItem > List;

    //! install help. Put items in the menu, create associated objects 
    static void install( const List& items )
    { items_ = items; }
    
    //! install help. Put items in the menu, create associated objects 
    static void install( const char *text[] );
    
    //! install help, from file
    static void install( const File& file );
    
    //! clear help
    static void clear( void )
    { items_.clear(); }
    
    //! modifiaction 
    static void setModified( const bool& value )
    { modified_ = value; }
    
    //! modification
    static bool& modified( void ) 
    { return modified_; }
    
    //! set dialog caption
    static void setWindowTitle( const std::string& value )
    { window_title_ = value; }
    
    //@}
    
    //! display action
    QAction& displayAction( void )
    { return *display_action_; }
    
    //! dump action
    QAction& dumpAction( void )
    { return *dump_action_; }
   
    protected:
    
    protected slots:
    
    //! opens help dialog
    void _display( void );
    
    //! dump help string in a TextEditor
    void _dumpHelpString( void );
    
    //! save help to file
    void _save( void );
    
    private:
    
    //!@name actions
    //@{
    
    //! dialog display action
    QAction* display_action_;
    
    //! dump help string
    QAction* dump_action_;
    
    //@}
        
    //! window title
    static std::string window_title_;
    
    //! help filename
    static File file_;

    //! modification state
    static bool modified_;
    
    //! complete set of help items
    static List items_;
    
  };
  
};

#endif
  
