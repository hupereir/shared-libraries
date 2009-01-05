#ifndef TransparencyHelpText_h
#define TransparencyHelpText_h

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

/*!
  \file    TransparencyHelpText.h
  \brief   transparency help text
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

//! transparency help text
namespace TRANSPARENCY
{
  static const QString HelpText[] = 
  {
  
    //_________________________________________________
    "Transparency",
    "<h2>Transparency</h2>"
    ""
    "<p>This section describes the various options used to set the application transparent look-and-feel. "
    "These options can be modified in the <i>Transparency</i> panel of the configuration dialog</p>"
    "<p>The following options are available:</p>"
    "<ul>"
    "<li style='margin-bottom:0.5em;'><i>use transparency</i> is used to enable/disable transparency globally.</li>"
    "<li style='margin-bottom:0.5em;'><i>use compositing</i> is used to enable/disable <i>true</i> transparency, when available (depending on the underlying operating system), "
    "provided that the <i>transparency</i> flag is used. When disabled, (or when compositing is not available), an attempt is made "
    "to use <i>fake</i> transparency, which consists of retrieving the desktop background picture, and merging it with the application "
    "<i>foreground</i> when drawing it on screen. The ability to retrieve the desktop background picture is also operating system dependent.</li>"
    "<li style='margin-bottom:0.5em;'><i>foreground color</i> is the default color used to display the application <i>foreground</i> objects (e.g. text). "
    "Some applications that have more complex objects to display use additional settings to allow for using multiple "
    "foreground colors. The foreground objects can appear semi-transparent, by <i>merging</i> the foreground color with the "
    "transparent background localed underneath. The amount by which these are merged is controlled by the <i>foreground intensity</i> option.</li>"
    "<li style='margin-bottom:0.5em;'><i>shadow color</i> is the color used to possibly draw a shadow below some <i>foreground</i> objects (e.g. text). "
    "This option is ignored if the <i>shadow offset</i> variable (see below) is set to zero.</li>"
    "<li style='margin-bottom:0.5em;'><i>tint color</i> is the color possibly used on top of the transparent background. The amount of color used for the tint is controlled by "
    "the <i>tint intensity</i> option. The color is ignored if this intensity is set to zero. On the contrary if the intensity is set to its maximum "
    "value, the transparent background is ignored, which is effectively equivalent to disabling all transparency effects.</li>"
    "<li style='margin-bottom:0.5em;'><i>highlight color</i> is the color possibly used on top of the transparent background, to highlight the application when the mouse pointer "
    "points to it. It behaves similarly to the tint color and the amount of highlighting is controlled by the <i>highlight intensity</i> option.</li>"
    "<li style='margin-bottom:0.5em;'><i>shadow offset</i> is the offset between foreground objects (e.g. text) and the shadow that is drawn underneath. Setting this parameter to zero disables "
    "the drawing of any shadow</li>"
    "<li style='margin-bottom:0.5em;'><i>foreground intensity</i> controls the amount of merging between the foreground color and the transparent background located "
    "underneath. A value of zero makes all foreground objects invisible. On the contrary a maximum value corresponds to a <i>solid</i> foreground "
    "color, meaning that the background below is not visible.</li>"
    "<li style='margin-bottom:0.5em;'><i>tint intensity</i> is the amount of tint color merged to the transparent background to form the application background. "
    "A value of zero correspond to no tint. A maximum value effectively disable transparency effects.</li>"
    "<li style='margin-bottom:0.5em;'><i>highlight intensity</i> is similar to the <i>tint intensity</i> option, excepts that it is used only when the application "
    "is pointed to by the mouse pointer.</li>"
    "<li style='margin-bottom:0.5em;'><i>opacity</i> controls the overall opacity of the application. This flag allows to set a <i>global</i> true opacity value for all objects constituting "
    "the application, disregarding all settings above. It is less general than the <use compositing</i> flag above, but works on a broader range "
    "of operating systems.</li>"
    "</ul>",
    0 
  };
    
};

#endif
