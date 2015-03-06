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
#include "CustomPixmap.h"
#include "IconEngine.h"
#include "IconSize.h"

#include <QColorDialog>
#include <QPixmap>
#include <QPainter>

//_________________________________________________________
ColorComboBox::ColorComboBox( QWidget* parent ):
    QComboBox( parent ),
    Counter( "ColorComboBox" )
{
    Debug::Throw( "ColorComboBox::ColorComboBox.\n" );
    setEditable( false );
    connect( this, SIGNAL(activated(int)), SLOT(_updateActiveIndex(int)) );
}

//_________________________________________________________
Base::Color::Set ColorComboBox::colors( void ) const
{
    
    Debug::Throw( "ColorComboBox::colors.\n" );
    
    Base::Color::Set out;
    for( int index = 0; index < QComboBox::count(); ++index )
    {
        
        const QVariant data( itemData( index ) );
        QColor color;
        if( data.canConvert<QColor>() ) color = data.value<QColor>();
        if( color.isValid() ) out.insert( color );

    }
    
    return out;
       
}

//_________________________________________________________
QColor ColorComboBox::color( void ) const
{ return itemData( currentIndex() ).value<QColor>(); }

//_________________________________________________________
void ColorComboBox::setColors( const Base::Color::Set& colors )
{
    
    Debug::Throw( "ColorComboBox::addColors.\n" );
   
    // first clear
    clear();
    
    // insert new action
    insertItem( QComboBox::count(), IconEngine::get( IconNames::Add ), tr( "New" ) );
    
    // insert default action
    insertItem( QComboBox::count(), CustomPixmap( IconSize( IconSize::Huge ), CustomPixmap::Transparent ), tr( "Default" ) );
    
    // loop over colors
    foreach( const Base::Color& color, colors )
    { addColor( color ); }
        
}

//_________________________________________________________
void ColorComboBox::addColor( const QColor& color )
{
    Debug::Throw() << "ColorComboBox::addColor - color: " << color.name() << endl;
    
    // create pixmap
    QPixmap pixmap = QPixmap( IconSize( IconSize::Huge ) );
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
    
    Debug::Throw() << "ColorComboBox::selectColor - color: " << color.name() << endl;
    if( !color.isValid() ) setCurrentIndex( 1 );
    else {
        
        bool found( false );
        for( int index = 2; index < QComboBox::count(); ++index )
        {
            const QColor local( itemData( index ).value<QColor>() );
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
    Debug::Throw() << "ColorComboBox::_updateActiveIndex - index: " << index << endl;
    if( index == 0 )
    {
        
        // select a new color
        QColor color( QColorDialog::getColor( Qt::white, this ) );
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
