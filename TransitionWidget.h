#ifndef TransitionWidget_h
#define TransitionWidget_h

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
  \file TransitionWidget.cpp
  \brief widget used for smooth transition between two widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPaintEvent>
#include <QPixmap>
#include <QTimeLine>
#include <QWidget>

#include "Counter.h"

class TransitionWidget: public QWidget, public Counter
{

  Q_OBJECT

  public:

  //! constructor
  TransitionWidget( QWidget* parent = 0 );

  //! destructor
  virtual ~TransitionWidget();

  //! flags
  enum Flag
  {
    NONE = 0,
    FROM_PARENT = 1<<0,
    SHOW = 1<<1
  };

  //! flags
  TransitionWidget& setFlags( unsigned int value )
  {
    flags_ = value;
    return *this;
  }

  //! flags
  TransitionWidget& setFlag( Flag flag, const bool& value = true )
  {
    if( value ) { flags_ |= flag; }
    else { flags_ &= (~flag); }
    return *this;
  }

  //! flags
  const unsigned int& flags( void ) const
  { return flags_; }

  //! set starting widget
  void initialize( QWidget* widget = 0, QRect rect = QRect() );

  //! start
  void start( void );

  //! timeline
  QTimeLine& timeLine( void )
  { return time_line_; }

  //! enable on options
  void setEnableOnOptions( bool value )
  { enable_on_options_ = value; }

  //! enability
  void setEnabled( bool value )
  { enabled_ = value; }

  //! enability
  const bool& isEnabled( void ) const
  { return enabled_; }

  protected:

  //! mouse press event
  /*!
  on mouse press, widget is automatically hidden. This prevents cases where "normal"
  hiding (at end of animation) fails for whatever reason
  */
  virtual void mousePressEvent( QMouseEvent* );

  //! paint event
  virtual void paintEvent( QPaintEvent* );

  private slots:

  //! configuration
  void _updateConfiguration( void );

  private:

  //! use options to enable
  bool enable_on_options_;

  //! enability
  bool enabled_;

  //! flags
  unsigned int flags_;

  //! timeline
  QTimeLine time_line_;

  //! current widget pixmap
  QPixmap pixmap_;

};

#endif
