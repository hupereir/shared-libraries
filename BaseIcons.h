#ifndef BaseIcons_h
#define BaseIcons_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file BaseIcons.h
  \brief namespace for base icons name wrapper
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <string>

//! namespace for base icons static name wrapper
namespace ICONS
{
  
  static const std::string ABOUT_QT = "qt-logo.png";
  static const std::string DIALOG_OK = "dialog-ok.png";
  static const std::string DIALOG_OK_APPLY = "dialog-ok-apply.png";
  static const std::string DIALOG_ACCEPT = DIALOG_OK_APPLY;
  static const std::string DIALOG_CANCEL = "dialog-cancel.png";
  static const std::string DIALOG_CLOSE = "dialog-close.png";
  static const std::string EDIT = "edit.png";
  static const std::string UNDO= "undo.png";
  static const std::string REDO= "redo.png";
  static const std::string EDIT_CLEAR = "clear_left.png"; 
  static const std::string CUT = "editcut.png";
  static const std::string COPY = "editcopy.png";
  static const std::string EXIT = "exit.png";
  static const std::string PASTE = "editpaste.png";
  static const std::string FIND = "viewmag.png";
  static const std::string FIND_LARGE = "viewmag-large.png";
  static const std::string COLOR = "color.png";
  static const std::string COLOR_PICKER = "colorpicker.png";
  static const std::string CONFIGURE = "configure.png";
  static const std::string HELP = "toggle_log.png";
  static const std::string OPEN = "fileopen.png";
  static const std::string PIN = "stock_pin.png";
  static const std::string RELOAD = "reload.png";
  static const std::string DELETE = "trashcan_empty.png";
  static const std::string WARNING = "messagebox_warning.png";
  static const std::string INFORMATION = "dialog-information.png";

};

#endif
