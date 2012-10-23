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
#include "PathHistory.h"

#include <QtGui/QAbstractButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QResizeEvent>
#include <QtGui/QStackedWidget>
#include <QtGui/QAbstractItemView>
#include <QtGui/QWidget>

//! path editor
class PathEditorItem;
class PathEditorMenuButton;
class PathEditor: public QStackedWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditor( QWidget* );

    //! destructor
    virtual ~PathEditor( void )
    {}

    //!@name modifiers
    //@{

    //! prefix
    void setPrefix( const QString& );

    //! home
    void setHomePath( const File& );

    //! root
    void setRootPathList( const File::List& );

    //! set path
    void setPath( const File&, const File& = File() );

    //@}

    //!@name accessors
    //@{

    //! path
    File path( void ) const;

    //! true if parent directory is available
    bool hasParent( void ) const;

    //! true if previous path is available in history
    bool hasPrevious( void ) const
    { return history_.previousAvailable(); }

    //! true if next path available in history
    bool hasNext( void ) const
    { return history_.nextAvailable(); }

    //! minimum size hint
    virtual QSize minimumSizeHint( void ) const;

    //! size hint
    virtual QSize sizeHint( void ) const
    { return minimumSizeHint(); }

    //@}

    signals:

    //! path changed
    void pathChanged( const File& );

    public slots:

    //! select parent path
    void selectParent( void );

    //! select previous path in history
    void selectPrevious( void );

    //! select next path in history
    void selectNext( void );

    protected:

    //! resize events
    virtual void resizeEvent( QResizeEvent* );

    //! prefix
    void _setUsePrefix( bool value )
    {
        if( usePrefix_ == value ) return;
        usePrefix_ = value;
        _updatePrefix();
    }

    //! update prefix
    void _updatePrefix( void );

    //! truncation
    void _setUseTruncation( bool );

    //! reload
    void _reload( void )
    { setPath( path() ); }

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

    private slots:

    //! configuration
    void _updateConfiguration( void );

    private:

    //! dummy treeview, needed for rendering items
    QAbstractItemView* itemView_;

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

    //!@name prefix
    //@{

    //! use prefix
    bool usePrefix_;

    //! prefix
    QString prefix_;

    //! prefix
    QLabel* prefixLabel_;

    //@}

    //!@name home directory
    //@{

    //! truncation
    bool truncate_;

    //! home directory
    File home_;

    //! root directories
    File::List rootPathList_;

    //! menu button
    PathEditorMenuButton* menuButton_;

    //! item buttons
    QList<PathEditorItem*> items_;

    //! path history
    PathHistory history_;

};

#endif
