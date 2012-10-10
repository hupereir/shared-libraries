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

#include <QString>

//! namespace for base icons static name wrapper
namespace ICONS
{

    static const QString ABOUT_QT = "qt-logo.png";
    static const QString ADD="list-add.png";
    static const QString COLOR = "format-fill-color.png";
    static const QString COLOR_PICKER = "color-picker.png";
    static const QString CONFIGURE = "configure.png";
    static const QString COPY = "edit-copy.png";
    static const QString CUT = "edit-cut.png";
    static const QString DELETE = "user-trash.png";
    static const QString DIALOG_CANCEL = "dialog-cancel.png";
    static const QString DIALOG_CLOSE = "dialog-close.png";
    static const QString DIALOG_OK = "dialog-ok.png";
    static const QString DIALOG_OK_APPLY = "dialog-ok-apply.png";
    static const QString DIALOG_ACCEPT = DIALOG_OK_APPLY;
    static const QString EDIT = "edit.png";
    static const QString EDIT_CLEAR = "edit-clear-locationbar-rtl.png";
    static const QString EXIT = "application-exit.png";
    static const QString FIND = "edit-find.png";
    static const QString HELP = "help-contents.png";
    static const QString LOCK = "document-encrypt.png";
    static const QString OPEN = "document-open.png";
    static const QString OPEN_FOLDER = "document-open-folder.png";
    static const QString PASTE = "edit-paste.png";
    static const QString PIN = "stock_pin.png";
    static const QString PRINT = "document-print.png";
    static const QString PRINT_PREVIEW = "document-print-preview.png";
    static const QString REDO= "edit-redo.png";
    static const QString RELOAD = "view-refresh.png";
    static const QString REMOVE="list-remove.png";
    static const QString RENAME="edit-rename.png";
    static const QString RUN="system-run.png";
    static const QString SHOW_MENU = "show-menu.png";
    static const QString UNDO= "edit-undo.png";

    static const QString INFORMATION = "dialog-information.png";
    static const QString WARNING = "dialog-warning.png";

    static const QString PREVIOUS = "arrow-left.png";
    static const QString NEXT = "arrow-right.png";
    static const QString HTML = "text-html.png";

    // configuration icons
    static const QString PREFERENCE_GENERAL="system-run.png";
    static const QString PREFERENCE_NAVIGATION="input-mouse.png";
    static const QString PREFERENCE_COLORS="preferences-desktop-color.png";
    static const QString PREFERENCE_LISTS="view-list-tree.png";
    static const QString PREFERENCE_MULTIPLE_VIEWS="view-split-left-right.png";
    static const QString PREFERENCE_TOOLBARS="configure-toolbars.png";
    static const QString PREFERENCE_RECENT_FILES="document-open-recent.png";
    static const QString PREFERENCE_FILE_TYPES="preferences-desktop-filetype-association.png";
    static const QString PREFERENCE_ANIMATIONS="tool-animator.png";
    static const QString PREFERENCE_EDITION="edit-rename.png";
    static const QString PREFERENCE_APPEARANCE="preferences-desktop-theme.png";
    static const QString PREFERENCE_TRANSPARENCY="preferences-desktop-theme.png";
    static const QString PREFERENCE_UNSORTED="preferences-other.png";
    static const QString PREFERENCE_PRINTING="document-print.png";
    static const QString PREFERENCE_NOTIFICATIONS="preferences-desktop-notification.png";
    static const QString PREFERENCE_MAIN_WINDOW="preferences-system-windows-actions.png";
    static const QString PREFERENCE_BACKGROUND="preferences-desktop-wallpaper.png";
    static const QString PREFERENCE_BOOKMARKS="bookmarks.png";
    static const QString PREFERENCE_SERVER="network-server.png";
    static const QString PREFERENCE_CACHE="preferences-web-browser-cache.png";
    static const QString PREFERENCE_PLAYERS="im-user.png";

};

#endif
