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

namespace Private
{
    class IconThemeDialog;
};

//* configuration dialog
class BaseConfigurationDialog: public TabbedDialog, public OptionWidgetList
{

    Q_OBJECT

    public:

    //* flag bitset for the Base configuration
    enum Flag
    {

        None = 0,
        Base = 1<<0,
        List = 1<<2,
        TextEdition = 1<<3,
        TabEmulation = 1<<4,
        ParagraphHighlight = 1<<5,
        TextEditionFlags = 1<<6,
        BoxSelection = 1<<7,
        Default = Base|List,
        AllTextEdition = TabEmulation|ParagraphHighlight|TextEditionFlags

    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //* constructor
    explicit BaseConfigurationDialog( QWidget* = nullptr, Flags = Flag::None );

    // read
    using OptionWidgetList::read;
    void read( void )
    {
        OptionWidgetList::read( XmlOptions::get() );
        _checkModified();
    }

    //* adds configuration box for base options used in all appications
    QWidget* baseConfiguration( QWidget* = nullptr, Flags = Flag::Default );

    //* list configuration box
    QWidget* listConfiguration( QWidget* = nullptr );

    //* TextEdit configuration box
    QWidget* textEditConfiguration( QWidget* = nullptr, Flags = Flag::AllTextEdition );

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

    //* emitted when configuration is changed
    void configurationChanged();

    protected Q_SLOTS:

    //* show pixmap path dialog
    void _editPixmapPathList( void );

    //* show icon path dialog
    void _editIconTheme( void );

    //* check options changed
    bool _checkModified( void );

    //* update configuration
    void _apply();

    //* save configuration from options
    void _save();

    //* restore configuration
    void _cancel();

    //* reset
    void _reset( void );

    //* restore default options
    void _restoreDefaults( void );

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
    Private::IconThemeDialog* iconThemeDialog_ = nullptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( BaseConfigurationDialog::Flags )

#endif
