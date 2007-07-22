// $Id$
#ifndef _ColorDisplay_h_
#define _ColorDisplay_h_

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
\file ColorDisplay.h
\brief used to display colors and a choose button
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QColorDialog>
#include <string>

#include "Counter.h"
#include "CustomLineEdit.h"

//! used to display colors and a choose button
class ColorDisplay: public QWidget, public Counter
{
  
  //! Qt metaobject macro
  Q_OBJECT
    
  public:
    
  //! constructor
  ColorDisplay( QWidget* parent );
  
  //! retrieve color
  QColor color( void ) const
  { return line_edit_.color(); }
  
  //! set color
  void setColor( const QColor& color )
  { line_edit_.setColor( color ); }
  
  private slots:
  
  //! change color (from button)
  void _changeColor( void )
  { 
    QColor color( QColorDialog::getColor( line_edit_.color(), this ) );
    if( color.isValid() ) setColor( color ); 
  }
  
  //! change color (from line editor)
  void _changeColorFromText( const QString& text = "" )
  { 
    QColor color( text.length() ? text:line_edit_.text() );
    if( color.isValid() ) setColor( color ); 
  }
    
  private:
  
  //! internal customized label to have correct background color
  class LocalLineEdit : public CustomLineEdit
  {
    
    public:
    
    //! constructor
    LocalLineEdit( QWidget *parent ):
      CustomLineEdit( parent )
    {}
      
    //! retrieve color
    QColor color( void ) const
    { return QColor( text() ); }
      
    //! set color
    void setColor( const QColor& color );
        
  };
  
  //! label used to display the color
  LocalLineEdit line_edit_;
  
};
#endif
