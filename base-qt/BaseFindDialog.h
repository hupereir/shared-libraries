#ifndef BaseFindDialog_h
#define BaseFindDialog_h

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

#include "AbstractFindWidget.h"
#include "BaseDialog.h"
#include "Counter.h"
#include "base_qt_export.h"

//* find dialog for text editor widgets
class BASE_QT_EXPORT BaseFindDialog: public BaseDialog, private Base::Counter<BaseFindDialog>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
        explicit BaseFindDialog( QWidget* = nullptr, Qt::WindowFlags = {} );

    //*@name accessors
    //@{

    //* string to find
    QString text() const
    { return baseFindWidget_->text(); }

    //* get selection
    virtual TextSelection selection( bool value ) const
    { return baseFindWidget_->selection( value ); }

    //* find widget
    AbstractFindWidget& baseFindWidget() const
    { return *baseFindWidget_; }

    //* retrieve editor
    QWidget& editor() const
    { return baseFindWidget_->editor(); }

    //@}

    //*@name modifiers
    //@{

    //* string to find
    void setText( const QString& text )
    { baseFindWidget_->setText( text ); }

    //* synchronize searched strings and ComboBox
    virtual void synchronize()
    { baseFindWidget_->synchronize(); }

    //* set base find widget
    void setBaseFindWidget( AbstractFindWidget* widget );

    //* take action when at least one match is found
    void matchFound()
    { baseFindWidget_->matchFound(); }

    //* take action when no match is found
    void noMatchFound()
    { baseFindWidget_->noMatchFound(); }

    //@}

    Q_SIGNALS:

    //* emitted when Find is pressed
    void find( TextSelection );

    private:

    //* find widget
    AbstractFindWidget* baseFindWidget_ = nullptr;

};
#endif
