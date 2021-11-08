#ifndef ToolBar_h
#define ToolBar_h

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

#include "Counter.h"
#include "Debug.h"
#include "OptionComboBox.h"
#include "base_qt_export.h"

#include <QHideEvent>
#include <QPaintEvent>
#include <QShowEvent>
#include <QToolBar>
#include <QHash>

//* customized tool bar to handle position and visibility from Options
class BASE_QT_EXPORT ToolBar: public QToolBar, private Base::Counter<ToolBar>
{

    Q_OBJECT

    public:

    //* constructor
    explicit ToolBar( const QString&, QWidget*, const QString& );

    // map toolbar area and name
    using AreaMap = QHash<QString, Qt::ToolBarArea>;

    //*@name convenience
    //@{
    //* get area from name
    static Qt::ToolBarArea nameToArea( const QString& name )
    {
        auto&& iter( _toolbarAreas().find( name ) );
        return iter == _toolbarAreas().end() ? (Qt::ToolBarArea) 0 :iter.value();
    }

    //* get name from toobar area
    static QString areaToName( Qt::ToolBarArea value )
    {

        for( auto&& iter = _toolbarAreas().begin(); iter != _toolbarAreas().end(); iter++ )
        { if( iter.value() == value ) return iter.key(); }

        return QString();

    }
    //@}

    //*@name accessors
    //@{

    //* lock from options
    bool lockFromOptions() const
    { return lockFromOptions_; }

    //* icon size from options
    bool iconSizeFromOptions() const
    { return iconSizeFromOptions_; }

    //* icon size from options
    bool toolButtonStyleFromOptions() const
    { return toolButtonStyleFromOptions_; }

    //* true if visible in menu
    bool appearsInMenu() const
    { return appearsInMenu_; }

    //* visibility action
    QAction& visibilityAction() const
    { return *visibilityAction_; }

    //@}

    //*@name modifiers
    //@{

    //* set transparent
    void setTransparent( bool value )
    { transparent_ = value; }

    //* set movable
    /* when called directly from the application, the lockFromOptions_ flag is set to false */
    void setMovable( bool value )
    {
        lockFromOptions_ = false;
        QToolBar::setMovable( value );
    }

    //* icon size
    /* when called directly from the application, the iconSizeFromOptions_ flag is set to false */
    void setIconSize( QSize size )
    {
        iconSizeFromOptions_ = false;
        QToolBar::setIconSize( size );
    }

    //* toolbutton style
    /* when called directly from the application, the toolButtonStyleFromOptions_ flag is set to false */
    void setToolButtonStyle( Qt::ToolButtonStyle style )
    {
        toolButtonStyleFromOptions_ = false;
        QToolBar::setToolButtonStyle( style );
    }

    //* true if visible in menu
    /** the flag is ignored if parent is MainWindow */
    void setAppearsInMenu( bool value )
    { appearsInMenu_ = value; }

    //@}


    //* location option combo box
    class BASE_QT_EXPORT LocationComboBox: public OptionComboBox
    {

        public:

        //* constructor
        explicit LocationComboBox( QWidget* parent, const QString& option ):
            OptionComboBox( parent, option )
        {
            addItem( ToolBar::areaToName( Qt::TopToolBarArea ) );
            addItem( ToolBar::areaToName( Qt::BottomToolBarArea ) );
            addItem( ToolBar::areaToName( Qt::LeftToolBarArea ) );
            addItem( ToolBar::areaToName( Qt::RightToolBarArea ) );
        }

    };

    protected:

    //* paint events
    void paintEvent( QPaintEvent* ) override;

    //* move event
    void moveEvent( QMoveEvent* ) override;

    private:

    //* toggle visibility
    void _toggleVisibility( bool );

    //* update configuration
    void _updateConfiguration();

    //* install actions
    void _installActions();

    //* assocated option name
    QString optionName_;
    QString locationOptionName_;

    //* visibility action
    QAction* visibilityAction_ = nullptr;

    //* true if toolbar should be transparent (no frame; no extra background)
    bool transparent_ = false;

    //* use icon size from options
    bool iconSizeFromOptions_ = true;

    //* toolbutton style from options
    bool toolButtonStyleFromOptions_ = true;

    //* use lock from options
    bool lockFromOptions_ = true;

    //* true if toolbar must appear in menu
    //* by default it is set to false in constructor if parent is either null or not a MainWindow
    bool appearsInMenu_ = true;

    //* map name and toolbar are
    static const AreaMap& _toolbarAreas();

};

#endif
