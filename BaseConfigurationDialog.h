#ifndef BaseConfigurationDialog_h
#define BaseConfigurationDialog_h

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

#include "TabbedDialog.h"
#include "XmlOptions.h"
#include "OptionWidget.h"
#include "OptionWidgetList.h"

#include <QLayout>

//! configuration dialog
class BaseConfigurationDialog: public TabbedDialog, public OptionWidgetList
{

    Q_OBJECT

    public:

    //! creator
    BaseConfigurationDialog( QWidget *parent = 0 );

    //! destructor
    virtual ~BaseConfigurationDialog()
    {}

    // read
    using OptionWidgetList::read;
    virtual void read( void )
    { OptionWidgetList::read( XmlOptions::get() ); }

    //! flag bitset for the Base configuration
    enum ConfigurationFlags
    {

        //! base (font, icons and debug level
        BASE = 1<<0,

        //! lists
        LIST = 1<<2,

        //! textEdit emulations
        TEXTEDIT = 1<<3,

        //! animations
        ANIMATIONS = 1<<4,

        //! tab emulation
        TAB_EMULATION = 1<<5,

        //! paragraph highlight
        PARAGRAPH_HIGHLIGHT = 1<<6,

        //! margins
        MARGINS = 1<<7,

        //! misc
        TEXT_EDITION_FLAGS = 1<<8,

        //! default
        DEFAULT = BASE|LIST|ANIMATIONS,

        //! all text edition
        ALL_TEXT_EDITION = TAB_EMULATION|PARAGRAPH_HIGHLIGHT|MARGINS|TEXT_EDITION_FLAGS

    };

    //! adds configuration box for base options used in all appications
    QWidget* baseConfiguration( QWidget* parent = 0, unsigned long flag = DEFAULT );

    //! list configuration box
    QWidget* listConfiguration( QWidget* parent = 0 );

    //! TextEdit configuration box
    QWidget* textEditConfiguration( QWidget* parent = 0, unsigned long flag = ALL_TEXT_EDITION );

    //! animations
    QWidget* animationConfiguration( QWidget* parent = 0 );

    Q_SIGNALS:

    //! modified
    void modified( void );

    //! apply button pressed
    void apply( void );

    //! ok button pressed
    void ok( void );

    //! canceled button pressed
    void cancel( void );

    //! emmited when configuration is changed
    void configurationChanged();

    protected Q_SLOTS:

    //! show pixmap path dialog
    virtual void _editPixmapPathList( void );

    //! show icon path dialog
    virtual void _editIconTheme( void );

    //! check options changed
    bool _checkModified( void );

    //! update configuration
    virtual void _apply();

    //! save configuration from options
    virtual void _save();

    //! restore configuration
    virtual void _cancel();

    //! reset
    virtual void _reset( void );

    //! restore default options
    virtual void _restoreDefaults( void );

    private:

    //! pointer to original options set
    /*!
    it is needed to keep track of the changes
    so that initial set is restored when pressing the cancel button
    */
    const Options backupOptions_;

    //!@name buttons
    //@{
    QPushButton* okButton_;
    QPushButton* applyButton_;
    QPushButton* cancelButton_;
    QPushButton* resetButton_;
    QPushButton* restoreDefaultsButton_;
    //@}
};


#endif
