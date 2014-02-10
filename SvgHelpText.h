#ifndef SvgHelpText_h
#define SvgHelpText_h

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

#include <QObject>

//! SVG help text
namespace Svg
{
    static const QString helpText[] =
    {

        //_________________________________________________
        QObject::tr( "Svg background" ),
        QObject::tr( "<h2>Svg background</h2>"
        "<h3>Configuration</h3>"
        "<p>This section describes the various options used to assign a SVG-based background to widgets in the application. "
        "These options can be modified in the <i>Background</i> panel of the configuration dialog</p>"
        "<p>The following options are available:</p>"
        "<ul>"
        "<li><i>use svg</i> enables the use of a SVG file to specify the background to be used. The SVG file must contain a specific set of elements ids"
        "listed below for the application to be able to use it.</li>"
        "<li><i>filename</i> is the name of the file from where the background is loaded.</li>"
        "</ul>"
        "<h3>File format specifications</h3>"
        "<p>A set of <i>default</i> SVG backgrounds are embedded in the applications. One can use alternative disk-resident files at will provided that the corresponding SVG "
        "contains the following element ids: </p>"
        "<ul>"
        "<li><i>top</i>, <i>left</i>, <i>right</i> and <i>bottom</i> for the sides of the background. top and bottom sides are stretched horizontally to match the widget width; "
        "left and right sides are stretched vertically to match the widget height.</li>"
        "<li><i>topleft</i>, <i>bottomleft</i>, <i>topright</i> and <i>bottomright</i> for the corners of the background. These are never stretched, whatever the widget size.</li>"
        "<li><i>center</i> the center of the background, stretched both horizontally and vertically, to match the widget size, together with the background sides.</li>"
        "</ul>"
        "<h3>Possible conflicts with transparency settings</h3>"
        "<p>It is recommended to enable the use of transparency when applying a SVG background to the widget, so that the transparent part of the background do-appear transparent with respect to "
        "the desktop, rather than with respect to the widget default background. However, this usually conflict with the use of a non-zero tint and highlight color among the various "
        "transparency settings. It is thus recommended to disable those by setting the corresponding intensity to 0 (see the relevant section of this help for further details).</p>" ),
        0
    };

};

#endif
