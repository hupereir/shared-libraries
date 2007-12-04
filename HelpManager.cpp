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
   \file HelpManager.cpp
   \brief reference manual help system
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QApplication>
#include <QAction>
#include <QFile>
#include <QVBoxLayout>

#include "BaseIcons.h"
#include "Debug.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlOptions.h"
#include "XmlUtil.h"

using namespace std;
using namespace Qt;
using namespace BASE;

//_________________________________________________________
// static members initialization
string HelpManager::window_title_( "Reference Manual" );
bool HelpManager::modified_( false );
HelpItem::List HelpManager::items_;
HelpItem::List HelpManager::backup_items_;
File HelpManager::file_;

//_________________________________________________________
HelpManager::HelpManager( QObject* parent ):
  QObject( parent ),
  Counter( "HelpManager" )
{ 
  
  Debug::Throw( "HelpManager::HelpManager.\n" );
  
  // default icon path
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  
  // actions
  display_action_ = new QAction( IconEngine::get( ICONS::HELP, path_list ), "&Reference Manual", 0 );
  connect( display_action_, SIGNAL( triggered() ), SLOT( _display() ) );
  
  dump_action_ = new QAction( "D&ump Help", 0 );
  connect( dump_action_, SIGNAL( triggered() ), SLOT( _dumpHelpString() ) );
  
  connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _save() ) );
  
}

//_________________________________________________________
void HelpManager::install( const char *text[] )
{

  Debug::Throw( "HelpManager::install.\n" );

  // clear existing text
  clear();
  
  //! loop over help text
  for( unsigned int i=0; text[i]; i++ ) {

    string label( text[i] );
    i++;
    if( !text[i] ) break;
    items_.push_back( HelpItem( label, text[i] ) );
  }

  return;

}

//_________________________________________________________
void HelpManager::install( const File& file )
{

  Debug::Throw( "HelpManager::Install.\n" );
  
  // set file and check
  file_ = file;
  if( !file_.exists() ) return;

  // parse the file
  QFile qtfile( file.c_str() );
  if ( !qtfile.open( QIODevice::ReadOnly ) )
  {
    Debug::Throw( "HelpManager::install - cannot open file.\n" );
    return;
  }

  // dom document
  QDomDocument document;
  XmlError error( file );
  if ( !document.setContent( &qtfile, &error.error(), &error.line(), &error.column() ) ) {
    qtfile.close();
    return;
  }
    
  // clear existing help
  clear();
  
  // loop over dom elements
  QDomElement doc_element = document.documentElement();
  QDomNode node = doc_element.firstChild();
  for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
  {
    QDomElement element = node.toElement();
    if( element.isNull() ) continue;

    string tag_name( qPrintable( element.tagName() ) );

    // special options
    if( tag_name == XML_ITEM ) items_.push_back( HelpItem( element ) );
    
  }
  
  return;

}
  
//_____________________________________________________
void HelpManager::_display( void )
{
  
  Debug::Throw( "HelpManager::_display.\n" );
  
  // create dialog
  HelpDialog* dialog( new HelpDialog( 0 ) );
  dialog->setWindowTitle( window_title_.c_str() );
  dialog->setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
  dialog->setItems( items_ );
  dialog->setEditEnabled( file_.size() );
  QtUtil::centerOnWidget( dialog, qApp->activeWindow() );
  dialog->show();
  return;
  
}

//_________________________________________________________
void HelpManager::_dumpHelpString( void )
{
  
  Debug::Throw( "HelpManager::_dumpHelpString.\n" );
  
  // write output to stream
  ostringstream out;
  
  // retrieve all items from dialog
  out << "static const char* HelpText[] = {\n";
  for( HelpItem::List::const_iterator iter = items_.begin(); iter != items_.end(); iter++ )
  {
    
    // dump label
    out << "  //_________________________________________________________\n"; 
    out << "  \"" << iter->label() << "\",\n";
    
    // dump text
    Str text( iter->text() );
    text = text.replace( "\"", "\\\"" );
    text = text.replace( "\n", "\\n\"\n  \"" );
    out << "  \"" << text << "\"";
    out << ",\n";
    out << "\n";
  }
  out << "  0\n";
  out << "};\n";
  
  QWidget* main( new QWidget(0) );
  main->setLayout( new QVBoxLayout() );
  main->layout()->setMargin(10);
  CustomTextEdit *text_edit = new CustomTextEdit( main );

  text_edit->setWrapFromOptions( false );
  text_edit->wrapModeAction().setChecked( false );
  text_edit->setPlainText( out.str().c_str() );
  main->layout()->addWidget( text_edit );
  main->resize( 600, 500 );
  
  // center 
  QtUtil::centerOnWidget( main, qApp->activeWindow() );
  
  main->show();
  
}

//_____________________________________________________
void HelpManager::_save( void )
{
  
  Debug::Throw() << "HelpManager::_save - file: " << file_ << endl;
  
  if( file_.empty() ) return;
  if( !modified() ) return;
  // output file
  QFile out( file_.c_str() );
  if( !out.open( QIODevice::WriteOnly ) ) return;
  
  // create document
  QDomDocument document;
  
  // top element
  QDomElement top = document.appendChild( document.createElement( XML_HELP.c_str() ) ).toElement();
  for( HelpItem::List::const_iterator iter = items_.begin(); iter != items_.end(); iter++ )
  { top.appendChild( iter->domElement( document ) ); }
 
  out.write( document.toByteArray() );
  out.close();
  setModified( false );
  return; 
}
