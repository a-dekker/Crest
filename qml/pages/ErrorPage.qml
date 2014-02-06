/******************************************************************************
 *                                                                            *
 * Crest - top like tool for SailfishOS                                       *
 * Copyright (C) 2014 by Michal Hrusecky <Michal@Hrusecky.net>                *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                            *
 ******************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: errorpage

    Column {
    anchors.centerIn: parent
    spacing: Theme.paddingLarge
    Label {
        width: errorpage.width
        color: Theme.highlightColor
        font.bold: true
        font.pixelSize: Theme.fontSizeLarge
        horizontalAlignment: Text.AlignHCenter
        text: "Something went wrong :-("
    }

    Text {
        wrapMode: Text.WordWrap
        color: Theme.primaryColor
        width: errorpage.width - 20
        x: 10
        text: "Either your system is a messed up, or you are running harbour version of Crest. Due to some Harbour policies, I can't use standard linux methods (requires) that reflects actual dependecies of this software and let package manager solve everything for you. I could bundle neccesarry software, but I'm not that type of guy, I prefer a clean solution and if not possible, work around it the cleanest way possible."
    }
    Text {
        wrapMode: Text.WordWrap
        width: errorpage.width - 20
        x: 10
        color: Theme.primaryColor
        text: "Long story short, to get Crest running run following command in terminal:"
    }
    Label {
        horizontalAlignment: Text.AlignHCenter
        width: errorpage.width
        color: Theme.highlightColor
        text: "pkcon install procps"
    }
    }
}


