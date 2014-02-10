#ifndef TransitionWidget_h
#define TransitionWidget_h

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

#include <QPaintEvent>
#include <QPixmap>
#include <QTimeLine>
#include <QWidget>

class TransitionWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    TransitionWidget( QWidget* parent = 0 );

    //! destructor
    virtual ~TransitionWidget();

    //! flags
    enum Flag
    {
        None = 0,
        FromParent = 1<<0,
        Show = 1<<1
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //! flags
    TransitionWidget& setFlags( Flags value )
    {
        flags_ = value;
        return *this;
    }

    //! flags
    TransitionWidget& setFlag( Flag flag, const bool& value = true )
    {
        if( value ) { flags_ |= flag; }
        else { flags_ &= (~flag); }
        return *this;
    }

    //! flags
    Flags flags( void ) const
    { return flags_; }

    //! set starting widget
    void initialize( QWidget* widget = 0, QRect rect = QRect() );

    //! start
    void start( void );

    //! timeline
    QTimeLine& timeLine( void )
    { return timeLine_; }

    //! enable on options
    void setEnableOnOptions( bool value )
    { enableOnOptions_ = value; }

    //! enability
    void setEnabled( bool value )
    { enabled_ = value; }

    //! enability
    const bool& isEnabled( void ) const
    { return enabled_; }

    protected:

    //! mouse press event
    /*!
    on mouse press, widget is automatically hidden. This prevents cases where "normal"
    hiding (at end of animation) fails for whatever reason
    */
    virtual void mousePressEvent( QMouseEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    private Q_SLOTS:

    //! configuration
    void _updateConfiguration( void );

    private:

    //! use options to enable
    bool enableOnOptions_;

    //! enability
    bool enabled_;

    //! flags
    Flags flags_;

    //! timeline
    QTimeLine timeLine_;

    //! current widget pixmap
    QPixmap pixmap_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( TransitionWidget::Flags );

#endif
