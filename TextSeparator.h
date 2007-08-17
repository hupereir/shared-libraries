// $Id$

#ifndef TextSeparator_h
#define TextSeparator_h

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
  \file TextSeparator.h
  \brief separators between words
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <set>
#include <QString>

//! text separator between words
class TextSeparator
{
  
  public:
  
  //! shortcut to set of separators      
  typedef std::set< QChar > SeparatorSet;
    
  //! return singleton
  static const TextSeparator& get( void )
  { return singleton_; }
  
  //! retrieve base separators
  const SeparatorSet& base() const
  { return base_separators_; }
  
  //! retrieve base separators
  const SeparatorSet& extended() const
  { return extended_separators_; }
  
  //! retrieve all separators
  const SeparatorSet& all() const
  { return separators_; }
  
  private: 
  
  //! constructor
  TextSeparator( void );
           
  //! singleton
  static TextSeparator singleton_;
  
  //! base separators (space, tab, end of line)
  SeparatorSet base_separators_;
  
  //! extended separators (brackets, parenthesis, quotes, etc.)
  SeparatorSet extended_separators_;
  
  //! all separators
  SeparatorSet separators_;
  
};

#endif
