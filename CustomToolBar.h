#ifndef CustomToolBar_h
#define CustomToolBar_h

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

#include "Counter.h"
#include "Debug.h"
#include "OptionComboBox.h"

#include <QHideEvent>
#include <QPaintEvent>
#include <QShowEvent>
#include <QToolBar>
#include <QMap>


//! customized tool bar to handle position and visibility from Options
class CustomToolBar: public QToolBar, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    CustomToolBar( const QString& = QString(), QWidget* = 0, const QString& = QString() );

    //! destructor
    virtual ~CustomToolBar( void );

    // map toolbar area and name
    typedef QMap< QString, Qt::ToolBarArea> AreaMap;

    //!@name convenience
    //@{
    //! get area from name
    static Qt::ToolBarArea nameToArea( const QString& name )
    {
        AreaMap::iterator iter = _toolbarAreas().find( name );
        return iter == _toolbarAreas().end() ? (Qt::ToolBarArea) 0 :iter.value();
    }

    //! get name from toobar area
    static QString areaToName( const Qt::ToolBarArea& value )
    {

        for( AreaMap::iterator iter = _toolbarAreas().begin(); iter != _toolbarAreas().end(); iter++ )
        { if( iter.value() == value ) return iter.key(); }

        return "";

    }
    //@}

    //!@name accessors
    //@{

    //! lock from options
    bool lockFromOptions( void ) const
    { return lockFromOptions_; }

    //! lock from options
    bool sizeFromOptions( void ) const
    { return sizeFromOptions_; }

    //! true if visible in menu
    /*! the flag is ignored if parent is MainWindow */
    bool appearsInMenu( void ) const
    { return appearsInMenu_; }

    //! visibility action
    QAction& visibilityAction( void ) const
    { return *visibilityAction_; }

    //@}

    //!@name modifiers
    //@{

    //! set transparent
    void setTransparent( bool value )
    { transparent_ = value; }

    //! set movable
    /* when called directly from the application, the lockFromOptions_ flag is set to false */
    void setMovable( bool value )
    {
        lockFromOptions_ = false;
        return QToolBar::setMovable( value );
    }

    //! icon size
    /* when called directly from the application, the sizeFromOptions_ flag is set to false */
    void setIconSize( QSize size )
    {
        sizeFromOptions_ = false;
        return QToolBar::setIconSize( size );
    }

    //! true if visible in menu
    /*! the flag is ignored if parent is MainWindow */
    void setAppearsInMenu( bool value )
    { appearsInMenu_ = value; }

    //@}


    //! location option combo box
    class LocationComboBox: public OptionComboBox
    {

        public:

        //! constructor
        LocationComboBox( QWidget* parent, const QString& option ):
            OptionComboBox( parent, option )
        {
            addItem( CustomToolBar::areaToName( Qt::TopToolBarArea ) );
            addItem( CustomToolBar::areaToName( Qt::BottomToolBarArea ) );
            addItem( CustomToolBar::areaToName( Qt::LeftToolBarArea ) );
            addItem( CustomToolBar::areaToName( Qt::RightToolBarArea ) );
        }

    };

    protected:

    //! paint events
    virtual void paintEvent( QPaintEvent* );

    //! show event
    virtual void showEvent( QShowEvent* );

    //! hide event
    virtual void hideEvent( QHideEvent* );

    //! move event
    virtual void moveEvent( QMoveEvent* );
    
    protected slots:

    //! toggle visibility
    virtual void _toggleVisibility( bool );

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! install actions
    void _installActions( void );

    //! assocated option name
    QString optionName_;
    QString locationOptionName_;
    
    //! visibility action
    QAction* visibilityAction_;

    //! true if toolbar should be transparent (no frame; no extra background)
    bool transparent_;

    //! use icon size from options
    bool sizeFromOptions_;

    //! use lock from options
    bool lockFromOptions_;

    //! true if toolbar must appear in menu
    /*! the flag is ignored if parent is MainWindow */
    bool appearsInMenu_;

    //! initialize area map
    static AreaMap _initializeAreas( void );

    //! map name and toolbar are
    static AreaMap& _toolbarAreas( void );

};

#endif
