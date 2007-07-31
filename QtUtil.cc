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
  \file QtUtil.cc
  \brief Qt utility static methods
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/


#include <qapplication.h>
#include <qcursor.h>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qfontmetrics.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qpainter.h>

#include "CustomPixmap.h"
#include "Debug.h"
#include "Exception.h"
#include "QtUtil.h"
#include "Util.h"
#include "XmlOptions.h"

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

using namespace std;

//____________________________________________________________
const unsigned int QtUtil::max_opacity_ = 0xffffffff;
const char* QtUtil::opacity_prop_name_ = "_NET_WM_WINDOW_OPACITY";

//____________________________________________________________
void QtUtil::infoDialog(  
  QWidget* parent, 
  const string& text,
  QtUtil::DialogCenter dialog_center )
{
  Debug::Throw( "QtUtil::infoDialog.\n" );
  QDialog dialog( parent );
  
  // create vbox layout
  QVBoxLayout* layout=new QVBoxLayout();
  layout->setMargin(10);
  layout->setSpacing(10);
  dialog.setLayout( layout );

  //! try load Question icon
  static CustomPixmap question_pixmap;
  static bool first( true );
  if( first )
  {
    first = false;
    list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
    question_pixmap.find( "messagebox_warning.png", path_list );    
  }
  
  // insert main vertical box
  if( question_pixmap.isNull() )
  { layout->addWidget( new QLabel( text.c_str(), &dialog ), 1, Qt::AlignHCenter ); }
  else
  {
    
    QHBoxLayout *h_layout( new QHBoxLayout() );
    layout->addLayout( h_layout, 1 );
    QLabel* label = new QLabel( &dialog );
    label->setPixmap( question_pixmap );
    h_layout->addWidget( label, 0, Qt::AlignHCenter );
    h_layout->addWidget( new QLabel( text.c_str(), &dialog ), 0, Qt::AlignHCenter );
    
  }
    
  // insert OK and Cancel button
  QPushButton *button( new QPushButton( "OK", &dialog ) );
  layout->addWidget( button, 0, Qt::AlignHCenter );
  dialog.connect( button, SIGNAL( clicked() ), &dialog, SLOT( accept() ) );

  layout->activate();
  
  // manage widget
  switch( dialog_center ) {
    case CENTER_ON_POINTER: 
      centerOnPointer( &dialog ); 
      break;
      
    case CENTER_ON_PARENT:  
      centerOnParent( &dialog ); 
      break;
    
    case CENTER_ON_DESKTOP: 
      centerOnDesktop( &dialog );
      break;
    
    default: break;
  }
  
  dialog.exec();
  return;
  
}

//____________________________________________________________
bool QtUtil::questionDialog(  
  QWidget* parent, 
  const string& text,
  QtUtil::DialogCenter dialog_center )
{
  
  Debug::Throw( "QtUtil::questionDialog.\n" );
  QDialog dialog( parent );
  
  // create vbox layout
  QVBoxLayout* layout=new QVBoxLayout();
  layout->setMargin(10);
  layout->setSpacing(10);
  dialog.setLayout( layout );
  
  //! try load Question icon
  static CustomPixmap question_pixmap;
  static bool first( true );
  if( first )
  {
    first = false;
    list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
    question_pixmap.find( "messagebox_warning.png", path_list );    
  }
  
  // insert main vertical box
  if( question_pixmap.isNull() )
  { layout->addWidget( new QLabel( text.c_str(), &dialog ), 1, Qt::AlignHCenter ); }
  else
  {
    
    QHBoxLayout *h_layout( new QHBoxLayout() );
    layout->addLayout( h_layout, 1 );
    QLabel* label = new QLabel( &dialog );
    label->setPixmap( question_pixmap );
    h_layout->addWidget( label, 0, Qt::AlignHCenter );
    h_layout->addWidget( new QLabel( text.c_str(), &dialog ), 0, Qt::AlignHCenter );
    
  }  
    
  // insert hbox layout for buttons
  QHBoxLayout *hbox_layout( new QHBoxLayout() );
  hbox_layout->setSpacing( 10 );
  layout->addLayout( hbox_layout );
  
  // insert OK button
  QPushButton *button = new QPushButton( "&Yes", &dialog );
  hbox_layout->addWidget( button );
  dialog.connect( button, SIGNAL( clicked() ), &dialog, SLOT( accept() ) );
  
  // insert Cancel button
  button = new QPushButton( "&No", &dialog );
  hbox_layout->addWidget( button );
  dialog.connect( button, SIGNAL( clicked() ), &dialog, SLOT( reject() ) );
  
  // manage widget
  layout->activate();

  // manage widget
  switch( dialog_center ) {
    case CENTER_ON_POINTER: 
      centerOnPointer( &dialog ); 
      break;
      
    case CENTER_ON_PARENT:  
      centerOnParent( &dialog ); 
      break;
    
    case CENTER_ON_DESKTOP: 
      centerOnDesktop( &dialog );
      break;
    
    default: break;
  }
  
  bool out = ( dialog.exec() == QMessageBox::Accepted );
  return out;
  
}

//____________________________________________________________
QRegion QtUtil::round( const QRect& region, const unsigned int& corners )
{
  
  QRegion out( region );
   
  // top left
  if( corners & TOP_LEFT )
  {
    const QPoint& point( region.topLeft() );
    out -= QRegion(point.x(), point.y()+0, 5, 1);
    out -= QRegion(point.x(), point.y()+1, 3, 1);
    out -= QRegion(point.x(), point.y()+2, 2, 1);
    out -= QRegion(point.x(), point.y()+3, 1, 2);
  }
  
  // top right
  if( corners & TOP_RIGHT )
  {
    const QPoint& point( region.topRight() );
    out -= QRegion(point.x()-4, point.y()+0, 5, 1);
    out -= QRegion(point.x()-2, point.y()+1, 3, 1);
    out -= QRegion(point.x()-1, point.y()+2, 2, 1);
    out -= QRegion(point.x()-0, point.y()+3, 1, 2);
  }
  
  // bottom left
  if( corners & BOTTOM_LEFT )
  {
    const QPoint& point( region.bottomLeft() );
    out -= QRegion(point.x(), point.y()-0, 5, 1);
    out -= QRegion(point.x(), point.y()-1, 3, 1);
    out -= QRegion(point.x(), point.y()-2, 2, 1);
    out -= QRegion(point.x(), point.y()-4, 1, 2);
  }
           
  
  // bottom right
  if( corners & BOTTOM_RIGHT )
  {
    const QPoint& point( region.bottomRight() );
    out -= QRegion(point.x()-4, point.y()-0, 5, 1);
    out -= QRegion(point.x()-2, point.y()-1, 3, 1);
    out -= QRegion(point.x()-1, point.y()-2, 2, 1);
    out -= QRegion(point.x()-0, point.y()-4, 1, 2);
  }
  
  return out;
  
}

//____________________________________________________________
QPixmap QtUtil::round( const QPixmap& pixmap, const unsigned int& corners )
{
  
  QPixmap out( pixmap );
  QPainter painter( &out );
  QColor color( QRgb(0) );
  painter.setBrush( color );
  
  // top left
  if( corners & TOP_LEFT )
  {
    const QPoint& point( out.rect().topLeft() );
    painter.drawRect(point.x(), point.y()+0, 5, 1);
    painter.drawRect(point.x(), point.y()+1, 3, 1);
    painter.drawRect(point.x(), point.y()+2, 2, 1);
    painter.drawRect(point.x(), point.y()+3, 1, 2);
  }
  
  // top right
  if( corners & TOP_RIGHT )
  {
    const QPoint& point( out.rect().topRight() );
    painter.drawRect(point.x()-4, point.y()+0, 5, 1);
    painter.drawRect(point.x()-2, point.y()+1, 3, 1);
    painter.drawRect(point.x()-1, point.y()+2, 2, 1);
    painter.drawRect(point.x()-0, point.y()+3, 1, 2);
  }
  
  // bottom left
  if( corners & BOTTOM_LEFT )
  {
    const QPoint& point( out.rect().bottomLeft() );
    painter.drawRect(point.x(), point.y()-0, 5, 1);
    painter.drawRect(point.x(), point.y()-1, 3, 1);
    painter.drawRect(point.x(), point.y()-2, 2, 1);
    painter.drawRect(point.x(), point.y()-4, 1, 2);
  }
           
  
  // bottom right
  if( corners & BOTTOM_RIGHT )
  {
    const QPoint& point( out.rect().bottomRight() );
    painter.drawRect(point.x()-4, point.y()-0, 5, 1);
    painter.drawRect(point.x()-2, point.y()-1, 3, 1);
    painter.drawRect(point.x()-1, point.y()-2, 2, 1);
    painter.drawRect(point.x()-0, point.y()-4, 1, 2);
  }
  
  return out;
  
}

//____________________________________________________________
void QtUtil::moveWidget( QWidget* widget, QPoint position )
{
  
  Debug::Throw( "QtUtil::moveWidget.\n" );
  if( !widget ) return;
  QDesktopWidget* desktop( qApp->desktop() );
  QRect geometry( desktop->screenGeometry( desktop->screenNumber( widget ) ) );
  if( position.y() + widget->height() > geometry.bottom()+1 ) position.setY( geometry.bottom() + 1 - widget->height() );  
  
  widget->move( position );
}

//____________________________________________________________
QPoint QtUtil::centerOnPointer( const QSize& size )
{
  Debug::Throw( "QtUtil::centerOnPointer.\n" );
  
  // get cursor position
  QPoint point( QCursor::pos() );
     
  point.setX( point.x() - size.width()/2 );
  point.setY( point.y() - size.height()/2 );
  
  // retrieve desktop
  QDesktopWidget *desktop( qApp->desktop() );
  
  // check point against desktop size
  if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
  if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );
  
  // check point against (0,0)
  if( point.x() < 0 ) point.setX( 0 );
  if( point.y() < 0 ) point.setY( 0 );  
  return point;
}


//____________________________________________________________
QPoint QtUtil::centerOnWidget( const QSize& size, QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnParent.\n" );
  if( !widget ) return centerOnDesktop( size );
  
  // get parent position and size
  QPoint point( widget->pos() );
  QSize parent_size( widget->frameSize() );
     
  point.setX( point.x() + ( parent_size.width() - size.width() )/2 );
  point.setY( point.y() + ( parent_size.height() - size.height() )/2 );
  
  // retrieve desktop
  QDesktopWidget *desktop( qApp->desktop() );
  
  // check point against desktop size
  if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
  if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );
  
  // check point against (0,0)
  if( point.x() < 0 ) point.setX( 0 );
  if( point.y() < 0 ) point.setY( 0 );  
  return point;
}
  
//____________________________________________________________
QPoint QtUtil::centerOnDesktop( const QSize& size )
{
  Debug::Throw( "QtUtil::centerOnDesktop.\n" );
  
  // retrieve desktop
  QDesktopWidget *desktop( qApp->desktop() );
  QSize desktop_size( desktop->frameSize() );
  
  // get parent position and size
  QPoint point( desktop->pos() );
     
  point.setX( point.x() + ( desktop_size.width() - size.width() )/2 );
  point.setY( point.y() + ( desktop_size.height() - size.height() )/2 );
  
  // check point against desktop size
  if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
  if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );
  
  // check point against (0,0)
  if( point.x() < 0 ) point.setX( 0 );
  if( point.y() < 0 ) point.setY( 0 );  
  return point;
}

//____________________________________________________________
void QtUtil::centerOnPointer( QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnPointer.\n" );
  Exception::check( widget, DESCRIPTION( "invalid widget" ) );
       
  // move widget
  widget->move( centerOnPointer( widget->frameSize() ) );
  qApp->processEvents();
  return;
}

//____________________________________________________________
void QtUtil::centerOnParent( QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnParent.\n" );
  Exception::check( widget, DESCRIPTION( "invalid widget" ) );
  
  // get parent widget
  QWidget* parent = widget->window();
  if( !parent ) centerOnDesktop( widget );
  else widget->move( centerOnWidget( widget->frameSize(), parent ) );
  return;
}

//____________________________________________________________
void QtUtil::centerOnDesktop( QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnDesktop.\n" );
  Exception::check( widget, DESCRIPTION( "invalid widget" ) );
  widget->move( centerOnDesktop( widget->frameSize() ) );
  return;
}

//____________________________________________________________
void QtUtil::expand( QLineEdit* line_edit, const string& ref_text )
{
  Debug::Throw( "QtUtil::expand.\n" );

  if( !line_edit ) return;
  string text( (ref_text.size() ) ? ref_text: qPrintable( line_edit->text() ) );
  QFontMetrics font( line_edit->fontMetrics() );
  int width( font.width( string( text+"   " ).c_str() ) );
  int extra_width( line_edit->frameSize().width() - line_edit->width() );
  line_edit->resize( QSize( width, 0 ) );
  line_edit->setMinimumSize( QSize( width + extra_width, 0 ) );
  return;
}

//____________________________________________________________
void QtUtil::uniconify( QWidget *widget )
{

  Debug::Throw( "QtUtil::uniconify.\n" );
  if( !widget->isTopLevel() ) return;

  #ifdef Q_WS_X11
  
  // this strongly uses X11 and is not portable
  XWMHints* h( XGetWMHints( QX11Info::display(), widget->winId() ) );
  h->initial_state = NormalState;
  XSetWMHints( QX11Info::display(), widget->winId(), h );
  XMapWindow( QX11Info::display(), widget->winId() );
  XRaiseWindow( QX11Info::display(), widget->winId() );
  // widget->show();
  // widget->raise();
  
  #else
  
  // this is portable but may not work on old enough X11 Qt versions
  if( widget->isMinimized() ) widget->hide();
  widget->show();
  widget->raise();
  
  #endif
  
  return;
  
}

//____________________________________________________________
void QtUtil::expand( QLabel* label, const string& ref_text )
{
  Debug::Throw( "QtUtil::expand.\n" );

  if( !label ) return;
  string text( (ref_text.size() ) ? ref_text: qPrintable( label->text() ) );
  QFontMetrics font( label->fontMetrics() );
  int width( font.width( string( text+"   " ).c_str() ) );
  int extra_width( label->frameSize().width() - label->width() );
  label->resize( QSize( width, 0 ) );
  label->setMinimumSize( QSize( width + extra_width, 0 ) );
  return;
}

//___________________________________________________________
unsigned int QtUtil::opacity( const QWidget* widget )
{
  
  Debug::Throw( "QtUtil::opacity.\n" );
      
  #ifdef Q_WS_X11
  
  Atom actual;
  int format;
  unsigned long n, left;
  unsigned char *data;
  XGetWindowProperty( QX11Info::display(), widget->winId(), XInternAtom( QX11Info::display(), opacity_prop_name_, false), 
      0L, 1L, false, XA_CARDINAL, &actual, &format, &n, &left, 
      (unsigned char **) &data);
  if (data != None)
  {
    unsigned int current_opacity;
    memcpy (&current_opacity, data, sizeof (unsigned int));
    XFree(( void *) data );
    return current_opacity;
  } else {
    
    std::cout << "TransparentWidget::PrintOpacity - atom not found" << std::endl;    
    return 1;
  }
  
  #else 
  
  return (unsigned int) max_opacity_*widget->opacity();
  
  #endif
}

//__________________________________________________________
void QtUtil::setOpacity( QWidget* widget, const double& value )
{
  
  Debug::Throw( "QtUtil::setOpacity.\n" );
  
  #ifdef Q_WS_X11
  
  unsigned int opacity = std::min( max_opacity_, static_cast<unsigned int>( max_opacity_*value ) );
  Debug::Throw() << "QtUtil::setOpacity - value: " << opacity << endl;
  
  if (opacity == max_opacity_ )  XDeleteProperty( QX11Info::display(), widget->winId(), XInternAtom(QX11Info::display(), opacity_prop_name_, false));
  else
  XChangeProperty( QX11Info::display(), widget->winId(), 
    XInternAtom(QX11Info::display(), opacity_prop_name_, false), 
    XA_CARDINAL, 32, PropModeReplace, 
    (unsigned char *) &opacity, 1L);
  XSync( QX11Info::display(), false );
  
  #else
  
  widget->setOpacity( value );
  
  #endif
 
}
