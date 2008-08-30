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

#include "BaseDialog.h"
#include "BaseIcons.h"
#include "Debug.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "XmlOptions.h"

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
  
  // actions
  display_action_ = new QAction( IconEngine::get( ICONS::HELP ), "&Reference Manual", this );
  connect( display_action_, SIGNAL( triggered() ), SLOT( _display() ) );
  
  dump_action_ = new QAction( "D&ump Help", this );
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

    QString label( text[i] );
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

    // special options
    if( element.tagName() == XML_ITEM ) items_.push_back( HelpItem( element ) );
    
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
  dialog->centerOnWidget( qApp->activeWindow() );
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
    out << "  \"" << qPrintable( iter->label() ) << "\",\n";
    
    // dump text
    QString text( iter->text() );
    text = text.replace( "\"", "\\\"" );
    text = text.replace( "\n", "\\n\"\n  \"" );
    out << "  \"" << qPrintable( text ) << "\"";
    out << ",\n";
    out << "\n";
  }
  out << "  0\n";
  out << "};\n";
  
  BaseDialog* dialog = new BaseDialog();
  dialog->setLayout( new QVBoxLayout() );
  dialog->layout()->setMargin(10);
  TextEditor *text_edit = new TextEditor( dialog );

  text_edit->setWrapFromOptions( false );
  text_edit->wrapModeAction().setChecked( false );
  text_edit->setPlainText( out.str().c_str() );
  dialog->layout()->addWidget( text_edit );
  dialog->resize( 600, 500 );
  dialog->centerOnWidget( qApp->activeWindow() );
  dialog->show();
  
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
  QDomElement top = document.appendChild( document.createElement( XML_HELP ) ).toElement();
  for( HelpItem::List::const_iterator iter = items_.begin(); iter != items_.end(); iter++ )
  { top.appendChild( iter->domElement( document ) ); }
 
  out.write( document.toByteArray() );
  out.close();
  setModified( false );
  return; 
}
