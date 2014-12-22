import QtQuick 2.3;

import RenderTools 1.0;

import "qrc:/js/settings.js" as Settings
import "qrc:/js/helpers.js" as Helpers

ModelViewer {
    id: modelViewer;

    fboSize: Qt.size(768, 768);

    onPost: {
        switch (message.header.reciever) {
        case "viewports" :
            if (message.data.action === "addPoint") {
                viewportArray.currentPoint = message.data.params.id;
            }
            break;
        }
    }

    ViewportArray {
        id: viewportArray;

        anchors.fill: parent;

        width: parent.width;
        height: parent.height;

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

            onRotate: modelViewer.recieve(message);

            currentPoint: viewportArray.currentPoint;

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

        onViewportsUpdated: {
            modelViewer.update();
        }

        function setZoom(zoomFactor, x, y, viewport) {
            zoom(zoomFactor, x, y, viewport);
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
