#ifndef BlockHighlight_h
#define BlockHighlight_h

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

#include "base_qt_export.h"
#include <QApplication>
#include <QBasicTimer>
#include <QTimerEvent>

#include "Counter.h"
#include "Debug.h"

class TextEditor;

//* handles current block highlighting
class BASE_QT_EXPORT BlockHighlight: public QObject, private Base::Counter<BlockHighlight>
{

  Q_OBJECT

  public:

  //* constructor
  explicit BlockHighlight( TextEditor* parent );

  //* enable/disable
  void setEnabled( bool value )
  { enabled_ = value; }

  //* enable/disable
  bool isEnabled() const
  { return enabled_; }

  //* clear highlighted block
  void clear();

  //* highlight
  void highlight();

  protected:

  //* delayed highlighting
  void timerEvent( QTimerEvent* event ) override;

  //* highlight
  void _highlight();

  private:

  //* trigger update associated editors
  void _updateEditors();

  //* parent editor
  TextEditor* parent_ = nullptr;

  //* associated timer
  QBasicTimer timer_;

  //* true if enabled
  bool enabled_ = false;

  //* true when cleared
  bool cleared_ = true;

};

#endif
