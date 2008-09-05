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
   \file FileRecord.h
   \brief handles previously opened file and tags
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "FileRecord.h"

using namespace std;

//_______________________________________________
FileRecord::PropertyId::Id FileRecord::PropertyId::counter_( 0 );
FileRecord::PropertyId::IdMap FileRecord::PropertyId::id_map_;
FileRecord::PropertyId::NameMap FileRecord::PropertyId::name_map_;

//_______________________________________________
FileRecord::PropertyId::Id FileRecord::PropertyId::get( std::string name )
{
    
  // see if iterator exists in list
  IdMap::const_iterator iter;
  if( ( iter = id_map_.find( name ) ) != id_map_.end() )
  { return iter->second; }
  
  // insert otherwise, increment counter and return proper value
  id_map_.insert( make_pair( name, counter_ ) );
  name_map_.push_back( name );
  counter_++;

  return counter_-1;
  
}

//_______________________________________________
std::string FileRecord::PropertyId::get( FileRecord::PropertyId::Id id )
{
  assert( id < name_map_.size() );
  return name_map_[id];
}

//_______________________________________________
bool FileRecord::operator < (const FileRecord& record ) const
{ return file() < record.file(); }

//_______________________________________________
bool FileRecord::operator == (const FileRecord& record ) const
{ return file() == record.file(); }

//_______________________________________________
FileRecord& FileRecord::addProperty( PropertyId::Id id, std::string value )
{
  
  properties_[ id ] = value; 
  return *this;
}
