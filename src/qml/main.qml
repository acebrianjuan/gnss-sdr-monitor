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

    MapQuickItem
    {
        id: marker
        coordinate: cttc
        anchorPoint.x: icon.width * 0.5
        anchorPoint.y: icon.height * 0.5

        sourceItem: Rectangle
        {
            id: icon
            width: 10
            height: 10
            radius: width/2
            color: "red"
        }
    }
}
