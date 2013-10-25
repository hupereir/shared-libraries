#ifndef TreeView_h
#define TreeView_h

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

#include "Counter.h"
#include "TextSelection.h"

#include <QAbstractItemDelegate>
#include <QMenu>
#include <QTreeView>
#include <QAction>

// forward declaration
class BaseFindDialog;
class ItemModel;

//! customized tree view
class TreeView: public QTreeView, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    TreeView( QWidget* = 0 );

    //! destructor
    virtual ~TreeView( void )
    {}

    //!@name accessors
    //@{

    //! TextSelection object from this selection, or clipboard
    TextSelection selection( void ) const;

    //! true if given index is visible
    bool isVisible( const QModelIndex& index ) const;

    //! number of visible columns
    virtual int visibleColumnCount( void ) const;

    //! option name
    virtual bool hasOptionName( void ) const
    { return !maskOptionName().isEmpty(); }

    //! option name
    virtual const QString& maskOptionName( void ) const
    { return maskOptionName_; }

    //! sort column option name
    virtual const QString& sortColumnOptionName( void ) const
    { return sortColumnOptionName_; }

    //! sort order option name
    virtual const QString& sortOrderOptionName( void ) const
    { return sortOrderOptionName_; }

    //! sort order option name
    virtual const QString& showHeaderOptionName( void ) const
    { return showHeaderOptionName_; }

    //! get mask
    virtual unsigned int mask( void ) const;

    //! set mask manually
    virtual void setMask( const unsigned int& );

    //@}

    //!@name modifiers
    //@{

    //! model
    virtual void setModel( QAbstractItemModel* );

    //! delegate
    virtual void setItemDelegate( QAbstractItemDelegate* );

    //! enable list finding
    void setFindEnabled( bool value );

    //! option name
    virtual bool setOptionName( const QString& );

    //! lock column
    void lockColumnVisibility( int index )
    { setColumnVisibilityLocked( index, true ); }

    //! lock/unlock column
    void setColumnVisibilityLocked( int index, bool value )
    {
        if( value ) lockedColumns_ |= (1<<index);
        else lockedColumns_ &= ~(1<<index);
    }

    //! true if column visibility is locked
    bool isColumnVisibilityLocked( int index ) const
    { return lockedColumns_&(1<<index); }

    //! load item margin from option
    void setItemMarginFromOptions( bool value )
    { itemMarginFromOptions_ = value; }

    //! load icon size from option
    void setIconSizeFromOptions( bool value )
    { iconSizeFromOptions_ = value; }

    //! force alternating row color
    void forceAlternatingRowColors( void )
    {
        forceAlternatingRowColors_ = true;
        setAlternatingRowColors( true );
    }

    //! set item margins
    virtual void setItemMargin( int );

    //! set icon size manually and disable option
    void setIconSize( const QSize& size )
    {
        QTreeView::setIconSize( size );
        iconSizeFromOptions_ = false;
    }

    //! store scrollbar position
    void storeScrollBarPosition( void );

    //! restore scrollbar position
    void restoreScrollBarPosition( void );

    //@}


    //!@name actions
    //@{

    //! select all
    QAction& selectAllAction( void ) const
    { return *selectAllAction_; }

    //! find from dialog
    QAction& findAction( void ) const
    { return *findAction_; }

    //! find selection again
    QAction& findSelectionAction( void ) const
    { return *findSelectionAction_; }

    //! find again
    QAction& findAgainAction( void ) const
    { return *findAgainAction_; }

    //! show header
    QAction& showHeaderAction( void ) const
    { return *showHeaderAction_; }

    //@}

    Q_SIGNALS:

    //! emmitted when index is hovered
    /*!
    this is similar to the 'entered' signal,
    except that it is also sent when no index is selected,
    for instance, to hide tooltips
    */
    void hovered( const QModelIndex& );

    //! emmitted when selection could not be found
    void noMatchFound( void );

    //! emmitted when selection could be found
    void matchFound( void );

    public Q_SLOTS:

    //! resize all visible columns to match contents
    virtual void resizeColumns()
    { resizeColumns( mask() ); }

    //! resize all visible columns to match contents
    virtual void resizeColumns( const unsigned int& mask );

    //! toggle show header
    virtual void toggleShowHeader( bool value );

    //!@name column mask
    //@{

    //! update column mask from option, if any
    virtual void updateMask( void );

    //! save column mask to option, if any
    virtual void saveMask( void );

    //@}

    //! sort order
    virtual void updateSortOrder( void );

    //! sort order
    virtual void saveSortOrder( void );

    //! find next occurence of TextSelection
    virtual void find( TextSelection selection );

    //! find current selection forward
    virtual void findSelectionForward( void )
    { _findForward( selection(), true ); }

    //! find current selection backward
    virtual void findSelectionBackward( void )
    { _findBackward( selection(), true ); }

    //! find last search forward
    virtual void findAgainForward( void );

    //! find last search forward
    virtual void findAgainBackward( void );

    //! store selected indexes in model
    void storeSelectedIndexes( void );

    //! restore selected indexes from model
    void restoreSelectedIndexes( void );

    //! store expanded indexes in model
    void storeExpandedIndexes( void );

    //! restore expanded indexes from model
    void restoreExpandedIndexes( void );

    protected:

    //! generic event
    virtual bool event( QEvent* );

    //! mouse move event
    virtual void mouseMoveEvent( QMouseEvent* );

    //! mouse press event
    virtual void mousePressEvent( QMouseEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! find dialog
    virtual BaseFindDialog& _findDialog( void )
    { return *findDialog_; }

    //! find dialog
    virtual void _createBaseFindDialog( void );

    //! find selection in forward direction
    virtual bool _findForward( const TextSelection& selection, bool rewind );

    //! find selection in backward direction
    virtual bool _findBackward( const TextSelection& selection, bool rewind );

    //! set item margin
    virtual void _setItemMargin( int );

    //! hover index
    virtual void _setHoverIndex( const QModelIndex& );

    protected Q_SLOTS:

    //! header menu
    virtual void _raiseHeaderMenu( const QPoint& );

    //! find text from dialog
    virtual void _findFromDialog( void );

    //! hover index
    virtual void _indexEntered( const QModelIndex& );

    private Q_SLOTS:

    //! update alternate item color
    void _updateConfiguration( void );

    private:

    //! install actions
    virtual void _installActions( void );

    //! return first index
    QModelIndex _firstIndex() const;

    //! return last index
    QModelIndex _lastIndex() const;

    //! return next index of current
    QModelIndex _indexAfter( const QModelIndex& ) const;

    //! return previous index
    QModelIndex _indexBefore( const QModelIndex& ) const;

    //! find dialog
    BaseFindDialog* findDialog_;

    //! model
    ItemModel* model_;

    //! hover index
    QModelIndex hoverIndex_;

    //!@name actions
    //@{

    //! select all items
    QAction* selectAllAction_;

    //! find from dialog
    QAction* findAction_;

    //! find selection again
    QAction* findSelectionAction_;

    //! find selection backward
    QAction* findSelectionBackwardAction_;

    //! find again
    QAction* findAgainAction_;

    //! find again backward
    QAction* findAgainBackwardAction_;

    //! show/hide header action
    QAction* showHeaderAction_;

    //@}

    //! mask option name
    QString maskOptionName_;

    //! sort column option name
    QString sortColumnOptionName_;

    //! sort order option name
    QString sortOrderOptionName_;

    //! header visibility
    QString showHeaderOptionName_;

    //! true if item margins are to be set from options
    bool itemMarginFromOptions_;

    //! true if icon size is to be set from options
    bool iconSizeFromOptions_;

    //! force alternating row colors
    bool forceAlternatingRowColors_;

    //! item margin
    int itemMargin_;

    //! selected column background color
    QColor selectedColumnColor_;

    //! vertical scrollbar position
    int vertical_;

    //! horizontal scrollbar position
    int horizontal_;

    //! locked columns
    unsigned lockedColumns_;

};

#endif
