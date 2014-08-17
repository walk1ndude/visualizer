import QtQuick 2.3

import RenderTools 1.0

import "../js/settings.js" as Settings
import "../js/helpers.js" as Helpers

Item {
    id: modelItem;

    property bool takeShot: false;

    property vector2d xRange: Qt.vector2d(0.0, 1.0);
    property vector2d yRange: Qt.vector2d(0.0, 1.0);
    property vector2d zRange: Qt.vector2d(0.0, 1.0);

    property vector3d rotation: Qt.vector3d(0.0, 0.0, 0.0);
    property real zoomFactor: 2.0;

    property int minHU: 0;
    property int maxHU: 65535;

    property string selectedPointName: "";
    property color selectedPointColor: Qt.rgba(0, 0, 0, 0);

    property int modelID: modelViewer.modelID;

    signal updateIndividualInfo();

    Rectangle {
        ModelViewer {
            id: modelViewer;
            objectName: "modelViewer";

            width: modelItem.width;
            height: modelItem.height;
            z: -2;

            fboSize: Qt.size(768, 768);

            xRange: modelItem.xRange;
            yRange: modelItem.yRange;
            zRange: modelItem.zRange;

            rotation: modelItem.rotation;

            zoomFactor: modelItem.zoomFactor;

            minHU: modelItem.minHU;
            maxHU: modelItem.maxHU;

            takeShot: modelItem.takeShot;

            selectedPointColor: modelItem.selectedPointColor;
            selectedPointName: modelItem.selectedPointName;

            onPointUpdated: {
                // update info about points
                Settings.Points[point.modelID] = Settings.Points[point.modelID] || { };
                Settings.Points[point.modelID][point.name] = Settings.Points[point.modelID][point.name] || { };
                Settings.Points[point.modelID][point.name]["position"] = point.position;

                // need to update sidebar info, so give singal about it
                updateIndividualInfo();
            }

            function updateViewPortInfo(viewPortArray) {
                //Helpers.createObjects(viewPortArray, Settings.ViewPorts, modelViewer, "/qml/ViewPort.qml");
            }

            ViewportArray {
                id: viewportArray;

                anchors.fill: parent;

                ViewportEx  {
                    projectionType: Viewport.PERSPECTIVE;
                    boundingRect: Qt.rect(0.5, 0.5, 0.5, 0.5);
                }

                ViewportEx {
                    projectionType: Viewport.TOP;
                    boundingRect: Qt.rect(0, 0.5, 0.5, 0.5);
                }

                ViewportEx {
                    projectionType: Viewport.FRONTAL;
                    boundingRect: Qt.rect(0, 0, 0.5, 0.5);
                }

                ViewportEx {
                    projectionType: Viewport.LEFT;
                    boundingRect: Qt.rect(0.5, 0, 0.5, 0.5);
                }
            }

            MouseArea {
                id: mouseAreaModelItem;
                property real prevMouseX: -1.0;
                property real prevMouseY: -1.0;

                enabled: false;

                anchors.fill: parent;
                onClicked: switch (mouse.button) {
                           case Qt.LeftButton: if (!!modelItem.selectedPointName) {
                               modelViewer.selectedPointPosition = Qt.point(
                                           mouseX * parent.fboSize.width / width,
                                           (height - mouseY) * parent.fboSize.height / height);
                               }

                               if (mouse.modifiers && Qt.ShiftModifier) {
                                   Helpers.mouseInViewPort(mouseX, height - mouseY, Settings.ViewPorts[modelViewer]);
                               }

                               break;
                           }

                onPressed: {
                    prevMouseX = mouseX;
                    prevMouseY = mouseY;
                }

                onPositionChanged: {
                    parent.mouseRotation(Qt.point(prevMouseX, height - prevMouseY), Qt.point(mouseX, height - mouseY));
                }

                onWheel: zoomFactor += wheel.angleDelta.y * 0.001;
            }
        }
    }

    onTakeShotChanged: {
        if (modelItem.takeShot) {
            angleShotTimer.running = true;
        }
    }

    function show() {
        mouseAreaModelItem.enabled = true;
    }
}
