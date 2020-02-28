#ifndef OptionWidgetList_h
#define OptionWidgetList_h

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

#include "base_qt_export.h"
#include "Debug.h"
#include "OptionWidget.h"

#include <QList>

#include <QApplication>
#include <QStyle>
#include <QStyleOption>

//* abstract container for OptionWidgets
template<class W>
class OptionWidgetList: public OptionWidget
{

    public:

    //* constructor
    explicit OptionWidgetList( W* buddy = nullptr ):
        OptionWidget( "generic" ),
        buddy_( buddy )
    {}

    //* add option widget
    template< class T >
        void addOptionWidget( T* );

    //* clear option widgets
    void clearOptionWidgets()
    {
        Debug::Throw( QStringLiteral("OptionWidgetList::clearOptionWidgets.\n") );
        optionWidgets_.clear();
    }

    //* read options
    void read( const Options& ) override;

    //* read options
    void write( Options& ) const override;

    protected:

    //* get necessary spacing for checkbox alignment
    int _checkBoxSpacing() const;

    private:

    //* buddy (needed to emit ::modified signal)
    W* buddy_ = nullptr;

    //* contained options
    QList< OptionWidget* > optionWidgets_;

};


//______________________________________________________________________
template<class W>
template<class T>
void OptionWidgetList<W>::addOptionWidget( T* widget )
{
    optionWidgets_.append( widget );
    if( buddy_ )  QObject::connect( widget, &T::modified, buddy_, &W::modified );
}

//______________________________________________________________________
template<class W>
void OptionWidgetList<W>::read( const Options& options )
{

    Debug::Throw( QStringLiteral("OptionWidgetList::read.\n") );
    for( const auto& widget:optionWidgets_ )
    { widget->read( options ); }

    _setConnected();

}

//______________________________________________________________________
template<class W>
void OptionWidgetList<W>::write( Options& options ) const
{
    Debug::Throw( QStringLiteral("OptionWidgetList::write\n") );
    for( const auto& widget:optionWidgets_ )
    { widget->write( options ); }
}

//__________________________________________________
template<class W>
int OptionWidgetList<W>::_checkBoxSpacing() const
{
    QStyleOption option;
    option.rect = QRect( 0, 0, 50, 50 );
    QRect adjusted( qApp->style()->subElementRect( QStyle::SE_CheckBoxContents, &option, nullptr ) );
    return adjusted.left()-option.rect.left();
}

#endif
