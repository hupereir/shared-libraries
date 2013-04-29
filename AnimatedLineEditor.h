#ifndef AnimatedLineEditor_h
#define AnimatedLineEditor_h

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

/*!
  \file AnimatedLineEditor.h
  \brief Animated text editor
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QTimeLine>

#include "LineEditor.h"

class TransitionWidget;

//! Animated text editor
/*! uses fading when calling setPlainText, setHTML or clear */
class AnimatedLineEditor: public LineEditor
{

  Q_OBJECT

  public:

  //! constructor
  AnimatedLineEditor( QWidget* parent = 0 );

  //! destrutor
  virtual ~AnimatedLineEditor( void );

  //! timeline
  QTimeLine& timeLine( void )
  { return time_line_; }

  public slots:

  //! clear
  virtual void clear( void );

  //! set text
  virtual void setText( const QString& );

  protected:

  //! transition widget
  TransitionWidget& _transitionWidget( void ) const
  { return *transition_widget_; }

  //! toggle clear button
  virtual bool _toggleClearButton( const bool& );

  //! paint clear button
  virtual void _paintClearButton( QPainter& );

  private slots:

  //! configuration
  virtual void _updateConfiguration( void );

  private:

  //! timeline
  QTimeLine time_line_;

  //! transition widget
  TransitionWidget* transition_widget_;


};

#endif
