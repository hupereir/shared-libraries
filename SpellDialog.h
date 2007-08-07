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

#ifndef SpellDialog_h
#define SpellDialog_h

/*!
  \file SpellDialog.h
  \brief spell checker popup dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QTextEdit>

#include <string>
#include <map>
#include <set>

#include "Counter.h"
#include "CustomListBox.h"
#include "CustomLineEdit.h"
#include "Exception.h"
#include "SpellInterface.h"
#include "TextPosition.h" 

namespace SPELLCHECK
{

  //! spell checker popup dialog
  class SpellDialog: public QDialog, public Counter
  {
  
    //! Qt meta object declaration
    Q_OBJECT
    
    public:
        
    //! constructor
    SpellDialog( QTextEdit* parent, const bool& read_only = false );
  
    //! destructor
    virtual ~SpellDialog();
    
    //! set read-only (enable/disable modification buttons)
    
    //! toggle filter combo box visibility
    virtual void showFilter( const bool& value );
   
    //! spell interface
    virtual SpellInterface& interface( void )
    { return interface_; }       
    
    //! editor
    QTextEdit &editor( void )
    { 
      Exception::checkPointer( editor_, DESCRIPTION( "editor_ not set" ) );
      return *editor_; 
    }
    
    //! dictionary
    bool setDictionary( const std::string& dict );
    
    //! dictionary
    std::string dictionary( void ) const
    { return qPrintable( dictionary_->itemText( dictionary_->currentIndex() ) ); }
    
    //! filter
    bool setFilter( const std::string& filter );
        
    //! dictionary
    std::string filter( void ) const
    { return qPrintable( filter_->itemText( filter_->currentIndex() ) ); }

    //! go to next word to be checked
    void nextWord( void );

    signals:
    
    //! ignore word
    void ignoreWord( const std::string& );
    
    //! need update
    void needUpdate( void );
    
    //! filter has changed
    void filterChanged( const std::string& );
    
    //! dictionary has changed
    void dictionaryChanged( const std::string& );
    
    protected slots:
  
    //! select suggestion, update replace_line_edit_
    virtual void _selectSuggestion(); 
  
    //! select suggestion, update replace_line_edit_
    virtual void _addWord( void ); 
    
    //! check word in Replace Editor
    virtual void _checkWord( void );
    
    //! select dictionary
    virtual void _selectDictionary( const QString& dict );
    
    //! select dictionary
    virtual void _selectFilter( const QString& filter );
    
    //! recheck text from start
    /*! initial begin/end are reset to the full text */
    virtual void _restart( void );
      
    //! ignore
    virtual void _ignore( void );
      
    //! ignore all
    virtual void _ignoreAll( void );
    
    //! replace
    virtual void _replace(  QListWidgetItem* item = 0 );
    
    //! replace All
    virtual void _replaceAll( void );
        
    protected:
            
    //! close
    virtual void closeEvent( QCloseEvent *e );
         
    private:
    
    //! update text editor selection
    void _updateSelection( const unsigned int&, const unsigned int& );
    
    //! replace text editor selection
    void _replaceSelection( const QString& );
   
    //! update suggestion list and editor for words
    void _displayWord( const QString& word );
    
    //! spell checking completed
    void _completed( void );
    
    //! spell interface
    SpellInterface interface_;
    
    //! text editor
    QTextEdit* editor_;
    
    //! initial readonly state
    bool read_only_editor_;
    
    //! line editor for original word
    CustomLineEdit *line_edit_;
            
    //! line editor for text replacement
    CustomLineEdit *replace_line_edit_;
    
    //! listbox for suggestions
    CustomListBox *suggestion_list_box_;
  
    //! combo box for dictionary
    QComboBox *dictionary_;
  
    //! filter label
    QLabel *filter_label_;
  
    //! combo box for filter
    QComboBox *filter_;
    
    //! state frame for message
    QLabel* state_label_;
        
    //! list of automatic replace words
    std::map< QString, QString > replaced_words_;
                
  };
};

#endif
