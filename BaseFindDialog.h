#ifndef BaseFindDialog_h
#define BaseFindDialog_h

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

#include "BaseDialog.h"
#include "BaseFindWidget.h"
#include "Counter.h"
#include "CustomComboBox.h"
#include "TextSelection.h"
#include "QOrderedSet.h"

#include <QAbstractButton>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QList>

//* find dialog for text editor widgets
class BaseFindDialog: public BaseDialog, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseFindDialog( QWidget* parent = nullptr, Qt::WindowFlags WindowFlags = 0 );

    //* destructor
    virtual ~BaseFindDialog( void ) = default;

    //*@name accessors
    //@{

    //* string to find
    virtual QString text( void ) const
    { return baseFindWidget_->text(); }

    //* get selection
    virtual TextSelection selection( bool value ) const
    { return baseFindWidget_->selection( value ); }

    //* find widget
    BaseFindWidget& baseFindWidget( void ) const
    { return *baseFindWidget_; }

    //* retrieve editor
    virtual CustomComboBox& editor( void ) const
    { return baseFindWidget_->editor(); }

    //@}

    //*@name modifiers
    //@{

    //* string to find
    virtual void setText( const QString& text )
    { baseFindWidget_->setText( text ); }

    //* synchronize searched strings and ComboBox
    virtual void synchronize( void )
    { baseFindWidget_->synchronize(); }

    //* enable/disable entire word
    virtual void enableEntireWord( bool value )
    { baseFindWidget_->enableEntireWord( value ); }

    //* enable/disable RegExp
    virtual void enableRegExp( bool value )
    { baseFindWidget_->enableRegExp( value ); }

    //@}

    Q_SIGNALS:

    //* emmited when Find is pressed
    void find( TextSelection );

    public Q_SLOTS:

    //* take action when at least one match is found
    virtual void matchFound( void )
    { baseFindWidget_->matchFound(); }

    //* take action when no match is found
    virtual void noMatchFound( void )
    { baseFindWidget_->noMatchFound(); }

    protected:

    //* button layout
    QBoxLayout& _buttonLayout() const
    { return *buttonLayout_; }

    //* "find" button
    QPushButton& _findButton( void ) const
    { return *findButton_; }

    private:

    //* find widget
    BaseFindWidget* baseFindWidget_ = nullptr;

    //* button layout
    QBoxLayout* buttonLayout_ = nullptr;

    //* find button
    QPushButton* findButton_ = nullptr;

};
#endif
