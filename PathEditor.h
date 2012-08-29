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
#include "File.h"

#include <QtCore/QEvent>
#include <QtCore/QList>

#include <QtGui/QButtonGroup>
#include <QtGui/QPaintEvent>
#include <QtGui/QAbstractButton>
#include <QtGui/QWidget>

//! path item
class PathEditorItem: public QAbstractButton, public Counter
{

    public:

    //! constructor
    PathEditorItem( QWidget* );

    //! destructor
    virtual ~PathEditorItem( void )
    {}

    //! set is last
    void setIsLast( bool value );

    //! set path
    void setPath( const File& );

    //! path
    const File& path( void ) const
    { return path_; }

    //! minimum size hint
    virtual QSize minimumSizeHint( void ) const;

    //! size hint
    virtual QSize sizeHint( void ) const;

    typedef QList<PathEditorItem*> List;

    protected:

    //! event
    virtual bool event( QEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! arrow width
    int _arrowWidth( void ) const;

    private:

    //! border width
    enum { BorderWidth = 2 };

    //! path
    File path_;

    //! true if is root
    bool isRoot_;

    //! true if last
    bool isLast_;

    //! true if hover
    bool mouseOver_;

};

//! path editor
class PathEditor: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PathEditor( QWidget* );

    //! destructor
    virtual ~PathEditor( void )
    {}

    //! set path
    void setPath( const File& );

    signals:

    //! path changed
    void pathChanged( const File& );

    protected slots:

    //! button clicked
    void _buttonClicked( QAbstractButton* );

    private:

    //! button group
    QButtonGroup* group_;

};

#endif
