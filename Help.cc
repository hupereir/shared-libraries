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
   \file Help.cc
   \brief reference manual system
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QFile>
#include <QLabel>
#include <QSplitter>
#include <QShortcut>
#include <QLayout>

#include "CustomTextEdit.h"
#include "Debug.h"
#include "Help.h"
#include "QtUtil.h"
#include "XmlOptions.h"
#include "XmlUtil.h"

using namespace std;
using namespace Qt;

//_________________________________________________________
//! xml tags
const std::string XML_HELP = "help";
const std::string XML_ITEM = "item";
const std::string XML_LABEL = "label";
const std::string XML_TEXT = "text";

//_________________________________________________________
HelpItem::HelpItem( const QDomElement& element ):
  Counter( "HelpItem" )
{

  Debug::Throw( "HelpItem::HelpItem.\n" );

  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    string name( qPrintable( attribute.name() ) );
    string value( qPrintable( attribute.value() ) );

    if( name == XML_LABEL ) label_ = XmlUtil::xmlToText(value);
    else Debug::Throw() << "HelpItem::HelpItem - unrecognized attribute: " << name << endl;
  }

  // parse children
  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    string tag_name( qPrintable( child_element.tagName() ) );
    if( tag_name == XML_TEXT ) text_ = XmlUtil::xmlToText( qPrintable( child_element.text() ) );
    else cout << "HelpItem::HelpItem - unrecognized child " << qPrintable( child_element.tagName() ) << endl;
  }

}

//_________________________________________________________
QDomElement HelpItem::DomElement( QDomDocument& parent ) const
{
  Debug::Throw( "HelpItem::DomElement.\n" );
  QDomElement out = parent.createElement( XML_ITEM.c_str() );
  out.setAttribute( XML_LABEL.c_str(), XmlUtil::textToXml( label_ ).c_str() );

  // text child
  if( text_.size() )
  out.
    appendChild( parent.createElement( XML_TEXT.c_str() ) ).
    appendChild( parent.createTextNode( XmlUtil::textToXml( text_ ).c_str() ) );

  return out;

}

//_________________________________________________________
HelpDialog::HelpDialog( QWidget *parent ):
  QMainWindow( parent ),
  Counter( "HelpDialog" ),
  edited_( false ),
  modified_( false ),
  current_item_( 0 )
{

  // splitter for help list and help display
  QSplitter *splitter( new QSplitter( this ) );
  splitter->setOrientation( Qt::Horizontal );
  setCentralWidget( splitter );

  // add help list
  list_ = new HelpList( splitter );

  // hbox for help editor and buttons
  QWidget *widget = new QWidget( splitter );
  widget->setLayout( new QVBoxLayout() );
  widget->layout()->setSpacing(2);
  widget->layout()->setMargin(0);

  // add text editor
  widget->layout()->addWidget( text_edit_ = new CustomTextEdit( widget ) );
  text_edit_->setReadOnly( true );
  QFont font;
  font.fromString( XmlOptions::get().raw( "FONT_NAME" ).c_str() );
  text_edit_->setFont( font );
  
  QList<int> sizes;
  sizes.push_back( 150 );
  sizes.push_back( 400 );
  splitter->setSizes( sizes );
  
  // button layout
  QHBoxLayout* layout = new QHBoxLayout();
  widget->layout()->addItem( layout );
  layout->setMargin( 5 );
  layout->setSpacing( 5 );

  layout->addWidget( edit_button_ = new QPushButton( "&Edit", widget ) );
  QtUtil::fixSize( edit_button_ );
  layout->addStretch( 1 );
  
  connect( edit_button_, SIGNAL( clicked() ), this, SLOT( _toggleEdition() ) );
  edit_button_->setToolTip( "edit current help" );

  // connect list to text edit
  connect( &list(), SIGNAL( itemSelectionChanged() ), this, SLOT( _display() ) );

  // add close accelerator
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), this, SLOT( close() ) );
  connect( new QShortcut( CTRL+Key_S, this ), SIGNAL( activated() ), this, SLOT( _save() ) );
  resize( 550, 400 );
}

//_________________________________________________________
void HelpDialog::addItem( const HelpItem& item )
{

  Debug::Throw( "HelpDialog::AddItem.\n" );
  new HelpList::Item( list_, item );
  return;

}

//_________________________________________________________
void HelpDialog::closeEvent( QCloseEvent *e )
{
  Debug::Throw( "HelpDialog::closeEvent" );
  _save();  
}

//_________________________________________________________
void HelpDialog::_display()
{

  Debug::Throw( "HelpDialog::_Display.\n" );
  
  // save modifications to current item, if needed
  _save();
  
  QList<HelpList::Item*> items( list().selectedItems<HelpList::Item>() );
  if( items.empty() ) return;
  
  current_item_ = items.front();
  text_edit_->setHtml( current_item_->item().text().c_str() );
  
  return;

}

//_________________________________________________________
void HelpDialog::_save( void )
{
  Debug::Throw( "HelpDialog::_Save.\n" );

  // update current item text if being edited
  if( edited_ && current_item_ )
  { 
    bool modified = !(current_item_->item().text() == qPrintable( text_edit_->toPlainText() ) );
    if( modified ) 
    {
      current_item_->item().setText( qPrintable( text_edit_->toPlainText() ) );
      modified_ = true;
      Debug::Throw( "HelpDialog::_Save - emit ItemModified.\n" );
      emit itemModified();
    }
  }
  
}

//_________________________________________________________
void HelpDialog::_toggleEdition( void )
{

  Debug::Throw( "HelpDialog::_ToggleEdition.\n" );
  if( !current_item_ ) return;
  
  if( !edited_ )
  {
    
    edited_ = true;
    edit_button_->setText( "&Display" );
    text_edit_->setPlainText( current_item_->item().text().c_str() );
  
  } else {

    // save modifications to current item, if needed
    _save();
  
    edited_ = false;
    edit_button_->setText( "&Edit" );
    text_edit_->setReadOnly( true );
    
    // update editor
    text_edit_->clear();
    text_edit_->setHtml( current_item_->item().text().c_str() );
    
  }

  return;

}

//_________________________________________________________
HelpManager::HelpManager():
  Counter( "HelpManager" )
{ 

  // create dialog
  dialog_ = new HelpDialog( 0 );
  dialog_->setWindowTitle( "Reference manual" );
  dialog_->setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
 
  // connections
  connect( dialog_, SIGNAL( itemModified() ), this, SLOT( save() ) );
  
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
  dialog_->list().setItemSelected( dialog_->list().item(0), true );
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
  QList<HelpList::Item*> items( dialog_->list().items<HelpList::Item>() );

  out << "static const char* HelpText[] = {\n";
  for( QList<HelpList::Item*>::const_iterator iter = items.begin(); iter != items.end(); iter++ )
  {
    const HelpList::Item& item( **iter );
    
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
 
  ofstream out( file_.c_str() );
  if( !out ) return;

  // retrieve list of items
  QList<HelpList::Item*> items( dialog_->list().items<HelpList::Item>() );
  
  // create document
  QDomDocument document;
  
  // top element
  QDomElement top = document.appendChild( document.createElement( XML_HELP.c_str() ) ).toElement();
  for( QList<HelpList::Item*>::iterator iter = items.begin(); iter != items.end(); iter++ )
  { top.appendChild( (*iter)->item().DomElement( document ) ); }
 
  out << qPrintable( document.toString() );
  out.close();
  return; 
}
