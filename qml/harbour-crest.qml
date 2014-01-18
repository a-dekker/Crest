import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import harbour.crest.ps 1.0

ApplicationWindow
{
    PS {
        id: ps
    }
    cover: Component { CoverPage { } }
    initialPage: Component { FirstPage { } }
}


