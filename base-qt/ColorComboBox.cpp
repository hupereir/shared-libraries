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

#include "ColorComboBox.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "Pixmap.h"


#include <QColorDialog>
#include <QPixmap>
#include <QPainter>

//_________________________________________________________
ColorComboBox::ColorComboBox( QWidget* parent ):
    QComboBox( parent ),
    Counter( QStringLiteral("ColorComboBox") )
{
    Debug::Throw( QStringLiteral("ColorComboBox::ColorComboBox.\n") );
    setEditable( false );
    connect( this, QOverload<int>::of( &ColorComboBox::activated ), this, &ColorComboBox::_updateActiveIndex );
}

//_________________________________________________________
Base::Color::Set ColorComboBox::colors() const
{

    Debug::Throw( QStringLiteral("ColorComboBox::colors.\n") );

    Base::Color::Set out;
    for( int index = 0; index < QComboBox::count(); ++index )
    {

        const QVariant data( itemData( index ) );
        QColor color;
        if( data.canConvert<QColor>() ) color = data.value<QColor>();
        if( color.isValid() ) out.insert( Base::Color(color) );

    }

    return out;

}

//_________________________________________________________
QColor ColorComboBox::color() const
{ return itemData( currentIndex() ).value<QColor>(); }

//_________________________________________________________
void ColorComboBox::setColors( const Base::Color::Set& colors )
{

    Debug::Throw( QStringLiteral("ColorComboBox::addColors.\n") );

    // first clear
    clear();

    // insert new action
    insertItem( QComboBox::count(), IconEngine::get( IconNames::Add ), tr( "New" ) );

    // insert default action
    insertItem( QComboBox::count(), Pixmap( IconSize::get( IconSize::Huge ), Pixmap::Flag::Transparent ), tr( "Default" ) );

    // loop over colors
    for( const auto& color:colors )
    { addColor( color ); }

}

//_________________________________________________________
void ColorComboBox::addColor( const QColor& color )
{
    Debug::Throw() << "ColorComboBox::addColor - color: " << color.name() << Qt::endl;

    // create pixmap
    QPixmap pixmap = QPixmap( IconSize::get( IconSize::Huge ) );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setPen( Qt::NoPen );
    painter.setRenderHints( QPainter::Antialiasing|QPainter::SmoothPixmapTransform );

    QRectF rect( pixmap.rect() );
    rect.adjust( 0.5, 0.5, -0.5, -0.5 );

    painter.setBrush( color );
    painter.setPen( Qt::NoPen );
    painter.drawEllipse( rect );
    painter.end();

    insertItem( QComboBox::count(), pixmap, color.name(), color );

}

//_________________________________________________________
void ColorComboBox::selectColor( const QColor&  color )
{

    Debug::Throw() << "ColorComboBox::selectColor - color: " << color.name() << Qt::endl;
    if( !color.isValid() ) setCurrentIndex( 1 );
    else {

        bool found( false );
        for( int index = 2; index < QComboBox::count(); ++index )
        {
            const auto local( itemData( index ).value<QColor>() );
            if( local == color )
            {
                setCurrentIndex( index );
                found = true;
                break;
            }

        }

        if( !found )
        {
            addColor( color );
            setCurrentIndex( QComboBox::count()-1 );
        }

    }

}

//_________________________________________________________
void ColorComboBox::_updateActiveIndex( int index )
{
    Debug::Throw() << "ColorComboBox::_updateActiveIndex - index: " << index << Qt::endl;
    if( index == 0 )
    {

        // select a new color
        const auto color( QColorDialog::getColor( Qt::white, this ) );
        if( color.isValid() )
        {

            addColor( color );
            lastColor_ = color;
            selectColor( color );

        } else {

            selectColor( lastColor_ );

        }

    }

}
