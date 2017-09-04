#ifndef TreeView_h
#define TreeView_h

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
#include "TextSelection.h"

#include <QAbstractItemDelegate>
#include <QMenu>
#include <QTreeView>
#include <QAction>

// forward declaration
class BaseFindDialog;
class BaseFindWidget;
class ItemModel;

//* customized tree view
class TreeView: public QTreeView, private Base::Counter<TreeView>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit TreeView( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* TextSelection object from this selection, or clipboard
    TextSelection selection() const;

    //* true if given index is visible
    bool isVisible( const QModelIndex& index ) const;

    //* number of visible columns
    int visibleColumnCount() const;

    //* option name
    bool hasOptionName() const
    { return !maskOptionName_.isEmpty(); }

    //* option name
    const QString& maskOptionName() const
    { return maskOptionName_; }

    //* sort column option name
    const QString& sortColumnOptionName() const
    { return sortColumnOptionName_; }

    //* sort order option name
    const QString& sortOrderOptionName() const
    { return sortOrderOptionName_; }

    //* sort order option name
    const QString& showHeaderOptionName() const
    { return showHeaderOptionName_; }

    //* get mask
    int mask() const;

    //* item margin
    int itemMargin() const
    { return itemMargin_; }

    //@}

    //*@name modifiers
    //@{

    //* model
    void setModel( QAbstractItemModel* ) override;

    //* delegate
    void setItemDelegate( QAbstractItemDelegate* );

    //* delegate
    void setItemDelegateForColumn( int, QAbstractItemDelegate* );

    //* enable list finding
    void setFindEnabled( bool value );

    //* option name
    bool setOptionName( const QString& );

    //* lock column
    void lockColumnVisibility( int index )
    { setColumnVisibilityLocked( index, true ); }

    //* lock/unlock column
    void setColumnVisibilityLocked( int index, bool value )
    {
        if( value ) lockedColumns_ |= (1<<index);
        else lockedColumns_ &= ~(1<<index);
    }

    //* true if column visibility is locked
    bool isColumnVisibilityLocked( int index ) const
    { return lockedColumns_&(1<<index); }

    //* load item margin from option
    void setItemMarginFromOptions( bool value )
    { itemMarginFromOptions_ = value; }

    //* load icon size from option
    void setIconSizeFromOptions( bool value )
    { iconSizeFromOptions_ = value; }

    //* force alternating row color
    void forceAlternatingRowColors()
    {
        forceAlternatingRowColors_ = true;
        setAlternatingRowColors( true );
    }

    //* set item margins
    void setItemMargin( int );

    //* set icon size manually and disable option
    void setIconSize( const QSize& size )
    {

        if( size != iconSize() )
        {
            QTreeView::setIconSize( size );

            #if QT_VERSION < 0x050000
            emit iconSizeChanged( size );
            #endif

        }

        iconSizeFromOptions_ = false;

    }

    //* store scrollbar position
    void storeScrollBarPosition();

    //* restore scrollbar position
    void restoreScrollBarPosition();

    //* set mask manually
    void setMask( int );

    //@}


    //*@name actions
    //@{

    //* select all
    QAction& selectAllAction() const
    { return *selectAllAction_; }

    //* find from dialog
    QAction& findAction() const
    { return *findAction_; }

    //* find selection again
    QAction& findSelectionAction() const
    { return *findSelectionAction_; }

    //* find again
    QAction& findAgainAction() const
    { return *findAgainAction_; }

    //* show header
    QAction& showHeaderAction() const
    { return *showHeaderAction_; }

    //@}

    //* container class for embedded Find dialog
    class Container: public QWidget, private Base::Counter<Container>
    {

        public:

        //* constructor
        explicit Container( QWidget* = nullptr );

        //* constructor
        explicit Container( QWidget*, TreeView* );

        //*@name accessors
        TreeView& treeView() const
        { return *treeView_; }

        private:

        //* initialize
        void _initialize();

        //* contained treeView
        TreeView* treeView_;

    };

    Q_SIGNALS:

    //* emitted when index is hovered
    /**
    this is similar to the 'entered' signal,
    except that it is also sent when no index is selected,
    for instance, to hide tooltips
    */
    void hovered( const QModelIndex& );

    //* emitted when selection could not be found
    void noMatchFound();

    //* emitted when selection could be found
    void matchFound();

    #if QT_VERSION < 0x050000
    //* icon size changed signal
    void iconSizeChanged( const QSize& );
    #endif

    public Q_SLOTS:

    //* resize all visible columns to match contents
    void resizeColumns()
    { resizeColumns( mask() ); }

    //* resize all visible columns to match contents
    void resizeColumns( int mask );

    //* toggle show header
    void toggleShowHeader( bool value );

    //*@name column mask
    //@{

    //* update column mask from option, if any
    void updateMask();

    //* save column mask to option, if any
    void saveMask();

    //@}

    //* sort order
    void updateSortOrder();

    //* sort order
    void saveSortOrder();

    //* find next occurence of TextSelection
    void find( TextSelection selection );

    //* find current selection forward
    void findSelectionForward()
    { _findForward( selection(), true ); }

    //* find current selection backward
    void findSelectionBackward()
    { _findBackward( selection(), true ); }

    //* find last search forward
    void findAgainForward();

    //* find last search forward
    void findAgainBackward();

    //* store selected indexes in model
    void storeSelectedIndexes();

    //* restore selected indexes from model
    void restoreSelectedIndexes();

    //* store expanded indexes in model
    void storeExpandedIndexes();

    //* restore expanded indexes from model
    void restoreExpandedIndexes();

    protected:

    //* generic event
    bool event( QEvent* ) override;

    //* mouse move event
    void mouseMoveEvent( QMouseEvent* ) override;

    //* mouse press event
    void mousePressEvent( QMouseEvent* ) override;

    //* paint event
    void paintEvent( QPaintEvent* ) override;

    //* find dialog
    void _createFindDialog();

    //* find widget
    void _createFindWidget( bool compact );

    //* find selection in forward direction
    bool _findForward( const TextSelection& selection, bool rewind );

    //* find selection in backward direction
    bool _findBackward( const TextSelection& selection, bool rewind );

    //* set item margin
    void _setItemMargin( int );

    //* hover index
    void _setHoverIndex( const QModelIndex& );

    protected Q_SLOTS:

    //* header menu
    void _raiseHeaderMenu( const QPoint& );

    //* find text from dialog
    void _findFromDialog();

    //* hover index
    void _indexEntered( const QModelIndex& );

    private Q_SLOTS:

    //* update alternate item color
    void _updateConfiguration();

    private:

    //* install actions
    void _installActions();

    //* return first index
    QModelIndex _firstIndex() const;

    //* return last index
    QModelIndex _lastIndex() const;

    //* return next index of current
    QModelIndex _indexAfter( const QModelIndex& ) const;

    //* return previous index
    QModelIndex _indexBefore( const QModelIndex& ) const;

    //* find dialog
    BaseFindDialog* findDialog_ = nullptr;

    //* find widget
    BaseFindWidget* findWidget_ = nullptr;

    //* model
    ItemModel* model_ = nullptr;

    //* hover index
    QModelIndex hoverIndex_;

    //*@name actions
    //@{

    //* select all items
    QAction* selectAllAction_ = nullptr;

    //* find from dialog
    QAction* findAction_ = nullptr;

    //* find selection again
    QAction* findSelectionAction_ = nullptr;

    //* find selection backward
    QAction* findSelectionBackwardAction_ = nullptr;

    //* find again
    QAction* findAgainAction_ = nullptr;

    //* find again backward
    QAction* findAgainBackwardAction_ = nullptr;

    //* show/hide header action
    QAction* showHeaderAction_ = nullptr;

    //@}

    //* mask option name
    QString maskOptionName_;

    //* sort column option name
    QString sortColumnOptionName_;

    //* sort order option name
    QString sortOrderOptionName_;

    //* header visibility
    QString showHeaderOptionName_;

    //* true if use dialog for finding
    bool useEmbeddedWidgets_ = false;

    //* true if item margins are to be set from options
    bool itemMarginFromOptions_ = true;

    //* true if icon size is to be set from options
    bool iconSizeFromOptions_ = true;

    //* force alternating row colors
    bool forceAlternatingRowColors_ = false;

    //* item margin
    int itemMargin_ = 2;

    //* selected column background color
    bool useSelectedColumnColor_ = false;

    //* vertical scrollbar position
    int vertical_ = 0;

    //* horizontal scrollbar position
    int horizontal_ = 0;

    //* locked columns
    int lockedColumns_ = 0;

    friend class Container;

};

#endif
