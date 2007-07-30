#ifndef _ColorMenu_h_
#define _ColorMenu_h_

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
  \file ColorMenu.h
  \brief color selection menu
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QMenu>
#include <QPixmap>

#include <string>
#include <set>

#include "Counter.h"
#include "Debug.h"

//! Color selection menu
class ColorMenu: public QMenu, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
    
  public: 

  //! colored pixmap size
  static const QSize PixmapSize;

  //! default icon name for color menu
  static const std::string COLOR_ICON;

  //! default name for no-color
  static const std::string NONE;

  //! used to sort colors in set
  class ColorLessFTor
  {
    public:
    
    //! predicate
    bool operator() (const QColor& first, const QColor& second ) const
    { return first.name() < second.name(); }
    
  };

  //! sorted set of colors
  typedef std::set<QColor, ColorLessFTor> ColorSet;

  //! constructor
  ColorMenu( QWidget* parent );
  
  //! destructor
  virtual ~ColorMenu( void ) 
  { Debug::Throw( "ColorMenu::~ColorMenu.\n" ); }
  
  //! add a color
  void add( const std::string& );

  //! retrieve colors
  ColorSet colors() const;
  
  signals:
  
  //! emmited when a color is selected    
  void selected( QColor );

  private slots:
     
  //! display colors when about to show
  void _display( void );

  //! adds a new color
  void _new( void );

  //! select the no color choice
  /*! emits colorSelected with an invalid color */
  void _default( void );

  //! get color associated to action, emit ColorSelected        
  void _selected( QAction* );
  
  private: 
 
  //! add a color
  void _add( const QColor& );

  //! map colors to display pixmap
  typedef std::map<QColor, QPixmap, ColorLessFTor> ColorMap;
    
  //! list of loaded colors
  ColorMap colors_;

  //! map actions to file recors
  std::map<QAction*, QColor> actions_;
         
};
#endif
