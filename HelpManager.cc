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
   \file HelpManager.cc
   \brief reference manual help system
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QFile>

#include "Debug.h"
#include "HelpManager.h"
#include "QtUtil.h"
#include "XmlOptions.h"
#include "XmlUtil.h"

using namespace std;
using namespace Qt;
using namespace HELP;


//_________________________________________________________
HelpManager::HelpManager():
  Counter( "HelpManager" )
{ 

  // create dialog
  dialog_ = new HelpDialog( 0 );
  dialog_->setWindowTitle( "Reference manual" );
  dialog_->setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
 
  // connections
  connect( dialog_, SIGNAL( itemModified() ), SLOT( save() ) );
  
}

//_________________________________________________________
bool HelpManager::install( const char *text[] )
{

  Debug::Throw( "HelpManager::install.\n" );

  // clear list of items
  dialog_->clear();

  //! loop over help text
  for( unsigned int i=0; text[i]; i++ ) {

    string label( text[i] );

    i++;
    if( !text[i] ) break;
    dialog_->addItem( HelpItem( label, text[i] ) );
  }

  return true;

}

//_________________________________________________________
bool HelpManager::install( const File& file )
{

  Debug::Throw( "HelpManager::Install.\n" );
  
  // set file and check
  file_ = file;
  if( !file_.exist() ) return false;

  // parse the file
  QFile qtfile( file.c_str() );
  if ( !qtfile.open( QIODevice::ReadOnly ) )
  {
    Debug::Throw( "HelpManager::install - cannot open file.\n" );
    return false;
  }

  // dom document
  QDomDocument document;
  XmlError error( file );
  if ( !document.setContent( &qtfile, &error.error(), &error.line(), &error.column() ) ) {
    qtfile.close();
    return false;
  }
    
  // clear list of items
  dialog_->clear();

  // loop over dom elements
  QDomElement doc_element = document.documentElement();
  QDomNode node = doc_element.firstChild();
  for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
  {
    QDomElement element = node.toElement();
    if( element.isNull() ) continue;

    string tag_name( qPrintable( element.tagName() ) );

    // special options
    if( tag_name == XML_ITEM ) dialog_->addItem( HelpItem( element ) );
  }
  
  return true;

}
  
//_____________________________________________________
void HelpManager::display( void )
{
  
  Debug::Throw( "HelpManager::display.\n" );
  dialog_->list().setCurrentItem( dialog_->list().item(0) );
  dialog_->setEditEnabled( file_.size() );
  dialog_->show();
  QtUtil::uniconify( dialog_ );

}

//_________________________________________________________
void HelpManager::dumpHelpString( void )
{
  
  // write output to stream
  ostringstream out;
  
  // retrieve all items from dialog
  QList<HelpItemList::Item*> items( dialog_->list().items<HelpItemList::Item>() );

  out << "static const char* HelpText[] = {\n";
  for( QList<HelpItemList::Item*>::const_iterator iter = items.begin(); iter != items.end(); iter++ )
  {
    const HelpItemList::Item& item( **iter );
    
    // dump label
    out << "  //_________________________________________________________\n"; 
    out << "  \"" << item.item().label() << "\",\n";
    
    // dump text
    Str text( item.item().text() );
    text = text.replace( "\"", "\\\"" );
    text = text.replace( "\n", "\\n\"\n  \"" );
    out << "  \"" << text << "\"";
    out << ",\n";
    out << "\n";
  }
  out << "  0\n";
  out << "};\n";
  
  CustomTextEdit *text_edit = new CustomTextEdit( 0 );
  text_edit->setReadOnly( true );
  text_edit->setWordWrapMode( QTextOption::NoWrap );
  text_edit->setPlainText( out.str().c_str() );
  text_edit->resize( 600, 500 );
  text_edit->show();
}

//_____________________________________________________
void HelpManager::save( void )
{
  
  Debug::Throw( "HelpManager::Save.\n" );
  if( file_.empty() ) return;
 
  // output file
  QFile out( file_.c_str() );
  if( !out.open( QIODevice::WriteOnly ) ) return;

  // retrieve list of items
  QList<HelpItemList::Item*> items( dialog_->list().items<HelpItemList::Item>() );
  
  // create document
  QDomDocument document;
  
  // top element
  QDomElement top = document.appendChild( document.createElement( XML_HELP.c_str() ) ).toElement();
  for( QList<HelpItemList::Item*>::iterator iter = items.begin(); iter != items.end(); iter++ )
  { top.appendChild( (*iter)->item().domElement( document ) ); }
 
  out.write( document.toByteArray() );
  out.close();
  return; 
}
