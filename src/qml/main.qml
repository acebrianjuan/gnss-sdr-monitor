import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6

Map
{
    property variant cttc: QtPositioning.coordinate(41.27504, 1.987709)

    id: map
    width: 512
    height: 512

    plugin: Plugin
    {
        id: mapPlugin
        name: "esri"
    }
    center: cttc
    zoomLevel: 15
}
