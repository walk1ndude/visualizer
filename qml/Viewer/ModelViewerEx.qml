import QtQuick 2.3;

import RenderTools 1.0;

import "qrc:/js/settings.js" as Settings
import "qrc:/js/helpers.js" as Helpers

ModelViewer {
    id: modelViewer;

    fboSize: Qt.size(768, 768);

    //xRange: Qt.vector2d(0.0, 1.0);
    //yRange: Qt.vector2d(0.0, 1.0);
    //zRange: Qt.vector2d(0.0, 1.0);

    selectedPoint: ({});

    onPointUpdated: {
        // update info about points
        Settings.Points[point.modelID] = Settings.Points[point.modelID] || { };
        Settings.Points[point.modelID][point.name] = Settings.Points[point.modelID][point.name] || { };
        Settings.Points[point.modelID][point.name]["position"] = point.position;
    }

    onPost: {
        if (message.header.reciever === "viewports") {
            if (message.data.action === "addPoint") {
                viewportArray.currentPoint = message.data.params.id;
            }
        }
    }

    ViewportArray {
        id: viewportArray;

        anchors.fill: parent;

        property string currentPoint: "";

        ViewportEx  {
            projectionType: Viewport.PERSPECTIVE;
            boundingRect: Qt.rect(0.5, 0.5, 0.5, 0.5);

            currentPoint: viewportArray.currentPoint;

            onSetZoom: parent.setZoom(zoomFactor, x, y, this);

            onPost: modelViewer.message = message;
        }

        ViewportEx {
            projectionType: Viewport.TOP;
            boundingRect: Qt.rect(0, 0.5, 0.5, 0.5);

            currentPoint: viewportArray.currentPoint;

            onSetZoom: parent.setZoom(zoomFactor, x, y, this);

            onPost: modelViewer.message = message;
        }

        ViewportEx {
            projectionType: Viewport.FRONTAL;
            boundingRect: Qt.rect(0, 0, 0.5, 0.5);

            currentPoint: viewportArray.currentPoint;

            onRotate: modelViewer.recieve(message);

            onSetZoom: parent.setZoom(zoomFactor, x, y, this);

            onPost: modelViewer.message = message;
        }

        ViewportEx {
            projectionType: Viewport.LEFT;
            boundingRect: Qt.rect(0.5, 0, 0.5, 0.5);

            currentPoint: viewportArray.currentPoint;

            onSetZoom: parent.setZoom(zoomFactor, x, y, this);

            onPost: modelViewer.message = message;
        }

        function setZoom(zoomFactor, x, y, viewport) {
            zoom(zoomFactor, x, y, viewport);

            modelViewer.update();
        }
    }

    modelScene: ModelScene {
        id: modelScene;

        viewportArray: viewportArray;

        blueprint: {
            "models" : [ {
                    "type" : "EvaluatorModel",
                    "params" : {
                        "width" : 10,
                        "height" : 10,
                        "stepX" : 10.0,
                        "color" : Qt.rgba(0, 0, 0.6)
                    }
                }
            ]
        }

        onScalingFactorChanged: modelViewerEx.scalingFactor = scalingFactor;
    }
}
