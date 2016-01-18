#ifndef BaseConfigurationDialog_h
#define BaseConfigurationDialog_h
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

#include "OptionWidget.h"
#include "OptionWidgetList.h"
#include "TabbedDialog.h"
#include "XmlOptions.h"

#include <QLayout>

class CustomDialog;
class IconThemeDialog;

//* configuration dialog
class BaseConfigurationDialog: public TabbedDialog, public OptionWidgetList
{

    Q_OBJECT

    public:

    //* flag bitset for the Base configuration
    enum ConfigurationFlag
    {

        None = 0,
        Base = 1<<0,
        List = 1<<2,
        TextEdition = 1<<3,
        TabEmulation = 1<<4,
        ParagraphHighlight = 1<<5,
        Margins = 1<<6,
        TextEditionFlags = 1<<7,
        Default = Base|List,
        AllTextEdition = TabEmulation|ParagraphHighlight|Margins|TextEditionFlags

    };

    Q_DECLARE_FLAGS( ConfigurationFlags, ConfigurationFlag );

    //* constructor
    BaseConfigurationDialog( QWidget* = nullptr, ConfigurationFlags = None );

    // read
    using OptionWidgetList::read;
    virtual void read( void )
    {
        OptionWidgetList::read( XmlOptions::get() );
        _checkModified();
    }

    //* adds configuration box for base options used in all appications
    QWidget* baseConfiguration( QWidget* = nullptr, ConfigurationFlags = Default );

    //* list configuration box
    QWidget* listConfiguration( QWidget* = nullptr );

    //* TextEdit configuration box
    QWidget* textEditConfiguration( QWidget* = nullptr, ConfigurationFlags = AllTextEdition );

    Q_SIGNALS:

    //* modified
    void modified( void );

    //* apply button pressed
    void apply( void );

    //* ok button pressed
    void ok( void );

    //* canceled button pressed
    void cancel( void );

    //* reset button pressed
    void reset( void );

    //* restore defaults button pressed
    void restoreDefaults( void );

    //* emmited when configuration is changed
    void configurationChanged();

    public Q_SLOTS:

    virtual void debugModification( void );

    protected Q_SLOTS:

    //* show pixmap path dialog
    virtual void _editPixmapPathList( void );

    //* show icon path dialog
    virtual void _editIconTheme( void );

    //* check options changed
    bool _checkModified( void );

    //* update configuration
    virtual void _apply();

    //* save configuration from options
    virtual void _save();

    //* restore configuration
    virtual void _cancel();

    //* reset
    virtual void _reset( void );

    //* restore default options
    virtual void _restoreDefaults( void );

    protected:

    //* find modification
    bool _findModification( const Options&, const Options& ) const;

    private:

    //* pointer to original options set
    /**
    it is needed to keep track of the changes
    so that initial set is restored when pressing the cancel button
    */
    const Options backupOptions_;

    //* pixmap path dialog
    CustomDialog* pixmapPathDialog_ = nullptr;

    //* icon theme dialog
    IconThemeDialog* iconThemeDialog_ = nullptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( BaseConfigurationDialog::ConfigurationFlags )

#endif
