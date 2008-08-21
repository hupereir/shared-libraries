#ifndef DictionaryMenu_h
#define DictionaryMenu_h

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
   \file DictionaryMenu.h
   \brief dictionary dictionary selection menu
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <map>
#include <string>

#include "Counter.h"

//! dictionary dictionary selection menu
namespace SPELLCHECK
{
  class DictionaryMenu: public QMenu, public Counter
  {
    
    Q_OBJECT
      
    public:
      
    //! constructor
    DictionaryMenu( QWidget* parent );
  
    //! select dictionary manualy
    void select( const QString& dictionary );
  
    signals:
  
    //! dictionary changed
    void selectionChanged( const QString& );
    
    protected slots:
    
    //! reset dictionary list
    void _reset( void );
    
    //! dictionary selected from menu
    void _selectDictionary( QAction* action );
    
    private:
    
    //! action group
    QActionGroup* group_;
    
    //! map dictionaries and action
    std::map<QAction*, QString> action_map_;
  
  };
  
};

#endif
