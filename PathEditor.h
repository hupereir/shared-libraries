#ifndef PathEditor_h
#define PathEditor_h

// $Id$

/*******************************************************************************
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
#include "CustomComboBox.h"
#include "File.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QTimerEvent>

#include <QtGui/QAbstractButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QResizeEvent>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

//! path editor
class PathEditor: public QStackedWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditor( QWidget* );

    //! destructor
    virtual ~PathEditor( void )
    {}

    //! prefix
    void setPrefix( const QString& );

    //! set path
    void setPath( const File& );

    //! path
    File path( void ) const;

    signals:

    //! path changed
    void pathChanged( const File& );

    public slots:

    //! prefix
    void setUsePrefix( bool value )
    {
        if( usePrefix_ == value ) return;
        usePrefix_ = value;
        _updatePrefix();
    }

    protected:

    //! resize events
    virtual void resizeEvent( QResizeEvent* );

    //! resize events
    virtual void timerEvent( QTimerEvent* );

    //! update prefix
    void _updatePrefix( void );

    protected slots:

    //! show browser
    void _showBrowser( void )
    { setCurrentWidget( browserContainer_ ); }

    //! show editor
    void _showEditor( void )
    { setCurrentWidget( editorContainer_ ); }

    //! return pressed in editor
    void _returnPressed( void );

    //! menu button clicked
    void _menuButtonClicked( void );

    //! update path from menu action
    void _updatePath( QAction* );

    //! button clicked
    void _buttonClicked( QAbstractButton* );

    //! update button visibility
    void _updateButtonVisibility( void );

    private:

    //! path browser container
    QWidget* browserContainer_;

    //! editor container
    QWidget* editorContainer_;

    //! editor
    CustomComboBox* editor_;

    //! button layout
    QBoxLayout* buttonLayout_;

    //! button group
    QButtonGroup* group_;

    //! resize timer
    QBasicTimer resizeTimer_;

    //! prefix
    QString prefix_;

    //! prefix
    QLabel* prefixLabel_;

    //! menu button
    QAbstractButton* menuButton_;

    //! use prefix
    bool usePrefix_;

};

#endif
