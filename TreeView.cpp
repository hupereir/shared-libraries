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

/*!
  \file TreeView.cpp
  \brief customized Tree View
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QCursor>
#include <QHeaderView>
#include <QPainter>
#include <QScrollBar>

#include "BaseFindDialog.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "ColorDisplay.h"
#include "ColumnSelectionMenu.h"
#include "ColumnSortingMenu.h"
#include "Singleton.h"
#include "TextEditor.h"
#include "TreeView.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;

//_______________________________________________
TreeView::TreeView( QWidget* parent ):
  QTreeView( parent ),
  Counter( "TreeView" ),
  find_dialog_( 0 ),
  menu_( 0 ),
  icon_size_from_options_( true )
{
  Debug::Throw( "TreeView::TreeView.\n" );   
    
  // actions
  _installActions();

  // default configuration
  setAllColumnsShowFocus( true );
  setRootIsDecorated( false );
  setSortingEnabled( true );
  
  #if QT_VERSION >= 0x040400
  setExpandsOnDoubleClick( false );
  #endif
  
  // header menu
  header()->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raiseHeaderMenu( const QPoint& ) ) );
  connect( header(), SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ), SLOT( saveSortOrder() ) );

  // configuration
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
     
}

//_______________________________________________
void TreeView::setFindEnabled( bool value )
{
  find_action_->setEnabled( value );
  find_selection_action_->setEnabled( value );
  find_again_action_->setEnabled( value );
  find_selection_backward_action_->setEnabled( value );
  find_again_backward_action_->setEnabled( value );
}

//_______________________________________________
QMenu& TreeView::menu( void )
{  
  
  if( !hasMenu() ) menu_ = new QMenu( this );  
  return *menu_;

}

//_______________________________________________
bool TreeView::isVisible( const QModelIndex& index ) const
{
  if( !( model() && index.isValid() ) ) return false;
  QModelIndex parent( model()->parent( index ) );
  return (!parent.isValid() ) || ( isExpanded( parent ) && isVisible( parent ) );
}

//_______________________________________________
int TreeView::visibleColumnCount( void ) const
{
  int out(0);
  for( int index=0; model() && index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) out++;
  return out;
}

//_______________________________________________
bool TreeView::setOptionName( const std::string& value )
{ 

  Debug::Throw( "TreeView::setOptionName.\n" );

  std::string tmp;
  
  // mask
  bool mask_changed( false );
  tmp = value + "_MASK";
  if( mask_option_name_ != tmp  ) 
  {
    mask_option_name_ = tmp;
    mask_changed = true;
    if( !XmlOptions::get().find( maskOptionName() ) ) saveMask();
    else updateMask(); 
  }
  
  // sort order
  bool sort_changed( false );
  tmp = value + "_SORT_ORDER";
  if( sort_order_option_name_ != tmp  ) 
  {
    sort_order_option_name_ = tmp;
    sort_changed = true;    
  }
  
  // sort column
  tmp = value + "_SORT_COLUMN";
  if( sort_column_option_name_ != tmp  ) 
  {
    
    sort_column_option_name_ = tmp;
    sort_changed = true;
    
  }

  // reload sorting
  if( sort_changed ) 
  {
    if( !( XmlOptions::get().find( sortOrderOptionName() ) && XmlOptions::get().find( sortColumnOptionName() ) ) ) saveSortOrder(); 
    else updateSortOrder();
  }
  
  return mask_changed || sort_changed;
  
}

//_______________________________________________
unsigned int TreeView::mask( void ) const
{
  Debug::Throw( "TreeView::mask.\n" );
  assert( model() );
  unsigned int mask = 0;
  for( int index=0; model() && index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) mask |= (1<<index);
  return mask;
}

//_______________________________________________
void TreeView::setMask( const unsigned int& mask )
{
  
  Debug::Throw( "TreeView::setMask.\n" );
  for( int index=0; index < model()->columnCount(); index++ )
  {
    
    // see if there is a change between new and old mask
    if( isColumnHidden( index ) == !(mask & (1<<index) ) ) continue;
    setColumnHidden( index, !(mask & (1<<index) ) );
    
  }  
  
  return;
  
}

//______________________________________________________
void TreeView::resizeColumns( const unsigned int& mask )
{

  // if no items present, do nothing
  if( !( model() && model()->rowCount() ) ) return;
  
  for( int i = 0; i < model()->columnCount(); i++ )
  { if( mask & (1<<i) ) resizeColumnToContents(i); }
  
}

//_______________________________________________
void TreeView::updateMask( void )
{

  Debug::Throw( "TreeView::updateMask.\n" );

  // check model and option availability
  if( !model() ) return;
  if( !hasOptionName() ) return;
  if( !XmlOptions::get().find( maskOptionName() ) ) return;
  
  // assign mask from options
  setMask( XmlOptions::get().get<unsigned int>( maskOptionName() ) );
  resizeColumns();
    
}

//_______________________________________________
void TreeView::saveMask( void )
{
  
  Debug::Throw( "TreeView::saveMask.\n" );
  if( !hasOptionName() ) return;
  XmlOptions::get().set<unsigned int>( maskOptionName(), mask() );

}

//_____________________________________________________________________
void TreeView::updateSortOrder( void )
{
  
  Debug::Throw( "TreeView::updateSortOrder.\n" );
  if( !hasOptionName() ) return;
  if( XmlOptions::get().find( sortColumnOptionName() ) && XmlOptions::get().find( sortColumnOptionName() ) )
  { sortByColumn( XmlOptions::get().get<int>( sortColumnOptionName() ), (Qt::SortOrder)(XmlOptions::get().get<int>( sortOrderOptionName() ) ) ); }
  
}

//_____________________________________________________________________
void TreeView::saveSortOrder( void )
{
  
  Debug::Throw( "TreeView::saveSortOrder.\n" );
  if( !hasOptionName() ) return;
  XmlOptions::get().set<int>( sortOrderOptionName(), header()->sortIndicatorOrder() ); 
  XmlOptions::get().set<int>( sortColumnOptionName(), header()->sortIndicatorSection() ); 
  
}

//______________________________________________________________________
void TreeView::find( TextSelection selection )
{
  Debug::Throw( "TreeView::find.\n" );
  bool found( selection.flag( TextSelection::BACKWARD ) ? _findBackward( selection, true ):_findForward( selection, true ) ); 
  if( found ) emit matchFound();
  else emit noMatchFound();
}

//______________________________________________________________________
void TreeView::findSelectionForward( void )
{ 
  Debug::Throw( "TreeView::findSelectionForward.\n" );
  _findForward( _selection(), true ); 
}

//______________________________________________________________________
void TreeView::findSelectionBackward( void )
{ 
  Debug::Throw( "TreeView::findSelectionBackward.\n" );
  _findBackward( _selection(), true ); 
}

//______________________________________________________________________
void TreeView::findAgainForward( void )
{ 
  Debug::Throw( "TreeView::findAgainForward.\n" );
  _findForward( TextEditor::lastSelection(), true ); 
}

//______________________________________________________________________
void TreeView::findAgainBackward( void )
{ 
  Debug::Throw( "TreeView::findAgainBackward.\n" );
  _findBackward( TextEditor::lastSelection(), true ); 
}

//__________________________________________________________
void TreeView::paintEvent( QPaintEvent* event )
{
 
  // check selected column background color
  if( !_selectedColumnColor().isValid() ) return QTreeView::paintEvent( event );
  
  // check number of columns
  if( visibleColumnCount() < 2 ) return QTreeView::paintEvent( event );
  if( header() && header()->isVisible() && header()->isSortIndicatorShown() )
  {

    // get selected column
    int selected_column( header()->sortIndicatorSection() );
    QRect rect( visualRect( model()->index( 0, selected_column ) ) );
    
    QPainter painter( viewport() );
    painter.setClipRect( event->rect() );
    rect.setTop(0);
    rect.setHeight( height() );
    painter.setBrush( _selectedColumnColor() );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rect );
    painter.end();
    
  }
  
  return QTreeView::paintEvent( event );
  
}

//__________________________________________________________
void TreeView::contextMenuEvent( QContextMenuEvent* event )
{
  
  Debug::Throw( "TreeView::contextMenuEvent.\n" );
  
  // check if menu was created
  if( !hasMenu() ) return;
      
  // move and show menu
  menu().adjustSize();
  menu().exec( event->globalPos() );
  
  // save mask after menu execution, 
  // to keep visible columns in sync with option
  saveMask();
  
}

//______________________________________________________________________
TextSelection TreeView::_selection( void ) const
{
  
  Debug::Throw( 0, "TreeView::_selection.\n" );

  // copy last selection
  TextSelection out( "" );
  out.setFlag( TextSelection::CASE_SENSITIVE, TextEditor::lastSelection().flag( TextSelection::CASE_SENSITIVE ) );
  out.setFlag( TextSelection::ENTIRE_WORD, TextEditor::lastSelection().flag( TextSelection::ENTIRE_WORD ) );

  QString text;
  if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) out.setText( text );
  else if( selectionModel() && model() && selectionModel()->currentIndex().isValid() )
  {
    
    Debug::Throw( 0, "TreeView::_selection - checking current.\n" );
    
    QModelIndex current( selectionModel()->currentIndex() );
    if( !(text =  model()->data( current ).toString()).isEmpty() ) out.setText( text );
    else if( allColumnsShowFocus() )
    {
   
      QModelIndex parent( model()->parent( current ) );
      
      // loop over all columns, retrieve 
      for( int column = 0; column < model()->columnCount( parent ); column++ )
      {
        // get index from column
        QModelIndex index( model()->index( current.row(), column, parent ) );
        if( index.isValid() && !(text =  model()->data( index ).toString()).isEmpty() )
        { 
          out.setText( text );
          break;
        }
        
      }
      
    }
  }
  
  // if everything else failed, retrieve last selection
  if( text.isEmpty() ) out.setText( TextEditor::lastSelection().text() );
  
  return out;

  
}
//______________________________________________________________________
void TreeView::_createBaseFindDialog( void )
{

  Debug::Throw( "TreeView::_createBaseFindDialog.\n" );
  if( !find_dialog_ )
  {

    find_dialog_ = new BaseFindDialog( this );
    connect( find_dialog_, SIGNAL( find( TextSelection ) ), SLOT( find( TextSelection ) ) );
    connect( this, SIGNAL( noMatchFound() ), find_dialog_, SLOT( noMatchFound() ) );
    connect( this, SIGNAL( matchFound() ), find_dialog_, SLOT( clearLabel() ) );

  }

  return;

}


//______________________________________________________________________
bool TreeView::_findForward( const TextSelection& selection, const bool& rewind )
{
  Debug::Throw( 0, "TreeView::_findForward.\n" );
  if( selection.text().isEmpty() ) return false;

  // store selection
  TextEditor::setLastSelection( selection );

  // useless
  return false;

}

//______________________________________________________________________
bool TreeView::_findBackward( const TextSelection& selection, const bool& rewind )
{

  Debug::Throw( 0, "TreeView::_findBackward.\n" );
  if( selection.text().isEmpty() ) return false;

  // store selection
  TextEditor::setLastSelection( selection );

  // useless
  return false;

}
//__________________________________________________________
void TreeView::_installActions( void )
{
  Debug::Throw( "TreeView::_installActions.\n" );

  addAction( select_all_action_ = new QAction( "Select All", this ) );
  select_all_action_->setShortcut( CTRL+Key_A );
  select_all_action_->setShortcutContext( WidgetShortcut );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );

  addAction( find_action_ = new QAction( IconEngine::get( ICONS::FIND ), "&Find", this ) );
  find_action_->setShortcut( Qt::CTRL + Qt::Key_F );
  find_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_action_, SIGNAL( triggered() ), SLOT( _findFromDialog() ) );

  addAction( find_again_action_ = new QAction( "F&ind Again", this ) );
  find_again_action_->setShortcut( Qt::CTRL + Qt::Key_G );
  find_again_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_again_action_, SIGNAL( triggered() ), SLOT( findAgainForward() ) );

  addAction( find_again_backward_action_ = new QAction( this ) );
  find_again_backward_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_G );
  find_again_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_again_backward_action_, SIGNAL( triggered() ), SLOT( findAgainBackward() ) );

  addAction( find_selection_action_ = new QAction( "Find &Selection", this ) );
  find_selection_action_->setShortcut( Qt::CTRL + Qt::Key_H );
  find_selection_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_selection_action_, SIGNAL( triggered() ), SLOT( findSelectionForward() ) );

  addAction( find_selection_backward_action_ = new QAction( this ) );
  find_selection_backward_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_H );
  find_selection_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_selection_backward_action_, SIGNAL( triggered() ), SLOT( findSelectionBackward() ) );

}

//___________________________________
void TreeView::_raiseHeaderMenu( const QPoint & pos )
{ 
  Debug::Throw( "TreeView::_raiseHeaderMenu.\n" ); 
  
  // check number of columns
  if( header()->count() <= 1 ) return;
  
  // create menu and raise.
  ColumnSelectionMenu menu( this, this );
  if( isSortingEnabled() ) 
  {
    menu.addSeparator();
    menu.addMenu( new ColumnSortingMenu( &menu, this ) );
  }

  menu.adjustSize();
  menu.exec( QCursor::pos() );
  
  // save mask after menu execution, 
  // to keep visible columns in sync with option
  saveMask();
  
}

//_____________________________________________________________________
void TreeView::_findFromDialog( void )
{
  Debug::Throw( "TreeView::_findFromDialog.\n" );

  // create
  if( !find_dialog_ ) _createBaseFindDialog();

  // enable/disable regexp
  _findDialog().enableRegExp( true );

  // raise dialog
  _findDialog().centerOnParent();
  _findDialog().show();

  /*
    setting the default text values
    must be done after the dialog is shown
    otherwise it may be automatically resized
    to very large sizes due to the input text
  */

  // set default string to find
  _findDialog().synchronize();
  _findDialog().clearLabel();

  // set default text
  // update find text
  QString text( _selection().text() );  
  if( !text.isEmpty() )
  {
    const int max_length( 1024 );
    text = text.left( max_length );
    _findDialog().setText( text );
  }
  
  // changes focus
  _findDialog().activateWindow();
  _findDialog().editor().setFocus();

  return;
}

//_____________________________________________________________________
void TreeView::_updateConfiguration( void )
{
  Debug::Throw( "TreeView::_updateConfiguration.\n" );
    
  // load mask from option, if any
  updateMask();
  updateSortOrder();
  
  // try load alternate colors from option
  QColor color;
  Str colorname( XmlOptions::get().get<string>("ALTERNATE_COLOR").c_str() );
  if( !colorname.isEqual( qPrintable( ColorDisplay::NONE ), false ) ) color = QColor( colorname.c_str() );
  
  if( !color.isValid() ) { setAlternatingRowColors( false ); }
  else {
    QPalette palette( this->palette() );
    palette.setColor( QPalette::AlternateBase, color );
    setPalette( palette );
    setAlternatingRowColors( true ); 
  }
  
  // try load selected column color from option
  color = QColor();
  colorname = Str( XmlOptions::get().get<string>("SELECTED_COLUMN_COLOR").c_str() );
  if( !colorname.isEqual( qPrintable( ColorDisplay::NONE ), false ) ) color = QColor( colorname.c_str() );
  _setSelectedColumnColor( color );
  
  // icon size
  if( icon_size_from_options_ )
  {
    int icon_size( XmlOptions::get().get<int>( "LIST_ICON_SIZE" ) );
    QTreeView::setIconSize( QSize( icon_size, icon_size )  );
  } 
    
}
