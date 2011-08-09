#ifndef CustomDockWidget_h
#define CustomDockWidget_h

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
#include "XmlOptions.h"

#include <QtGui/QDockWidget>
#include <QtGui/QStyle>

#include <map>

class CustomDockWidget: public QDockWidget, public Counter
{
    Q_OBJECT

    public:

    //! constructor
    CustomDockWidget( const QString&, QWidget * parent = 0, const QString& = "" );

    //! destructor
    virtual ~CustomDockWidget( void )
    {}

    // map toolbar area and name
    typedef std::map< QString, Qt::DockWidgetArea> AreaMap;

    //! get area from name
    static Qt::DockWidgetArea nameToArea( const QString& name )
    {
        AreaMap::iterator iter = _dockWidgetAreas().find( name );
        return iter == _dockWidgetAreas().end() ? (Qt::DockWidgetArea) 0 :iter->second;
    }

    //! get name from toobar area
    static QString areaToName( const Qt::DockWidgetArea& value )
    {

        for( AreaMap::iterator iter = _dockWidgetAreas().begin(); iter != _dockWidgetAreas().end(); iter++ )
        { if( iter->second == value ) return iter->first; }

        return "";

    }

    //! prefered location
    bool hasPreferredArea( void ) const
    { return optionName_.size() && XmlOptions::get().find( _locationOptionName() ); }

    //! prefered location
    Qt::DockWidgetArea preferredArea( void ) const
    {
        assert( optionName_.size() );
        return nameToArea( XmlOptions::get().raw( _locationOptionName() ) );
    }

    //! lock
    void setLocked( bool value );

    protected:

    //! location option name
    QString _locationOptionName( void ) const
    { return optionName_ + "_LOCATION"; }

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    //! update location
    void _updateLocation( Qt::DockWidgetArea );

    private:

    // Empty titlebar for the dock widgets when "Lock Layout" has been activated.
    class TitleBar : public QWidget
    {
        public:

        //! constructor
        TitleBar(QWidget* parent = 0):
            QWidget(parent)
        {}

        //! destructor
        virtual ~TitleBar()
        {}

        //! minimum size
        virtual QSize minimumSizeHint() const
        {
            const int border = style()->pixelMetric(QStyle::PM_DockWidgetTitleBarButtonMargin);
            return QSize(border, border);
        }

        //! size
        virtual QSize sizeHint() const
        { return minimumSizeHint(); }

    };

    //! assocated option name
    QString optionName_;

    //! title widget
    TitleBar* title_;

    //! locked
    bool locked_;

    //! initialize area map
    static AreaMap _initializeAreas( void );

    //! map name and toolbar are
    static AreaMap& _dockWidgetAreas( void );

};

#endif
