#ifndef PathEditor_h
#define PathEditor_h

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
#include "CustomComboBox.h"
#include "File.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QMenu>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QAbstractItemView>
#include <QWidget>

// forward declaration
namespace Private
{
    class PathEditorItem;
    class PathEditorMenuButton;
}

class PathHistory;

//* path editor
class PathEditor: public QStackedWidget, private Base::Counter<PathEditor>
{

    Q_OBJECT

    public:

    //* dragging
    static const QString MimeType;

    //* constructor
    explicit PathEditor( QWidget* );

    //*@name accessors
    //@{

    //* path
    File path( void ) const;

    //* true if parent directory is available
    bool hasParent( void ) const;

    //* true if previous path is available in history
    bool hasPrevious( void ) const;

    //* true if next path available in history
    bool hasNext( void ) const;

    //* minimum size hint
    QSize minimumSizeHint( void ) const override;

    //* size hint
    QSize sizeHint( void ) const override
    { return minimumSizeHint(); }

    //* previous path menu
    QMenu& previousPathMenu( void ) const
    { return *previousPathMenu_; }

    //* next path menu
    QMenu& nextPathMenu( void ) const
    { return *nextPathMenu_; }

    //@}

    //*@name modifiers
    //@{

    //* prefix
    void setPrefix( const QString& );

    //* history tag name
    void setHistoryTagName( const QString& );

    //* home
    void setHomePath( const File& );

    //* root
    void setRootPathList( const File::List& );

    //* true if filesystem is local
    void setIsLocal( bool );

    //* set dragEnabled
    void setDragEnabled( bool );

    //* set path
    void setPath( const File&, const File& = File() );

    //@}

    Q_SIGNALS:

    //* path changed
    void pathChanged( const File& );

    public Q_SLOTS:

    //* select parent path
    void selectParent( void );

    //* select previous path in history
    void selectPrevious( void );

    //* select next path in history
    void selectNext( void );

    //* select from action
    void selectFromMenu( QAction* );

    protected:

    //* resize events
    void resizeEvent( QResizeEvent* ) override;

    //* prefix
    void _setUsePrefix( bool value )
    {
        if( usePrefix_ == value ) return;
        usePrefix_ = value;
        _updatePrefix();
    }

    //* update prefix
    void _updatePrefix( void );

    //* truncation
    void _setUseTruncation( bool );

    //* reload
    void _reload( void )
    { setPath( path() ); }

    protected Q_SLOTS:

    //* show browser
    void _showBrowser( void )
    { setCurrentWidget( browserContainer_ ); }

    //* show editor
    void _showEditor( void )
    { setCurrentWidget( editorContainer_ ); }

    //* return pressed in editor
    void _returnPressed( void );

    //* menu button clicked
    void _menuButtonClicked( void );

    //* update path from menu action
    void _updatePath( QAction* );

    //* button clicked
    void _buttonClicked( QAbstractButton* );

    //* update button visibility
    void _updateButtonVisibility( void );

    //* update previous and next path menus
    void _updatePathMenus( void );

    private Q_SLOTS:

    //* configuration
    void _updateConfiguration( void );

    private:

    //* dummy treeview, needed for rendering items
    QAbstractItemView* itemView_ = nullptr;

    //* path browser container
    QWidget* browserContainer_ = nullptr;

    //* editor container
    QWidget* editorContainer_ = nullptr;

    //* editor
    CustomComboBox* editor_ = nullptr;

    //* button layout
    QBoxLayout* buttonLayout_ = nullptr;

    //* button group
    QButtonGroup* group_ = nullptr;

    //*@name prefix
    //@{

    //* use prefix
    bool usePrefix_ = true;

    //* prefix
    QString prefix_;

    //* prefix
    QLabel* prefixLabel_ = nullptr;

    //@}

    //* true if editor corresponds to local file system
    bool isLocal_ = true;

    //* truncation
    bool truncate_ = true;

    //* true if drag and drop is enabled (false by default)
    bool dragEnabled_ = false;

    //* home directory
    File home_;

    //* root directories
    File::List rootPathList_;

    //* menu button
    Private::PathEditorMenuButton* menuButton_ = nullptr;

    //* previous path menu
    QMenu* previousPathMenu_ = nullptr;

    //* nex path menu
    QMenu* nextPathMenu_ = nullptr;

    //* item buttons
    QList<Private::PathEditorItem*> items_;

    //* path history
    PathHistory* history_ = nullptr;

};

#endif
