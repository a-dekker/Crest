

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
import QtQuick 2.2
import Sailfish.Silica 1.0
import "pages"
import harbour.crest.ps 1.0

ApplicationWindow {
    id: app

    PS {
        id: ps
    }

    property var is_ok: ps.sys_check()

    allowedOrientations: Orientation.Portrait | Orientation.Landscape
                         | Orientation.LandscapeInverted
    _defaultPageOrientations: Orientation.Portrait | Orientation.Landscape
                              | Orientation.LandscapeInverted
    cover: Component {
        CoverPage {
        }
    }

    initialPage: Qt.resolvedUrl(
                     ps.sys_check(
                         ) ? "pages/FirstPage.qml" : "pages/ErrorPage.qml")
}
