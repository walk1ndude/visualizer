/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Item {
    id: slider
    width: 400
    height: 16
    property int position: 0
    property real value: (maximum - minimum) / width * coeff * position
    property int maximum: 255
    property int minimum: 0
    property real coeff: 1.0
    property int xMax: width - handle.width - 4

    onXMaxChanged: updatePos()
    onMinimumChanged: updatePos()

    function updatePos() {
        if (maximum > minimum) {
            var pos = 2 + (position - minimum) * slider.xMax / (maximum - minimum);
            pos = Math.min(pos, width - handle.width - 2);
            pos = Math.max(pos, 2);
            handle.x = pos;
        } else {
            handle.x = 2;
        }
    }

    Rectangle {
        anchors.fill: parent
        border.color: "white"
        border.width: 0
        radius: 8
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#66343434"
            }

            GradientStop {
                position: 1.0
                color: "#66000000"
            }
        }

    }

    MouseArea {
        id: mouseS
        anchors.fill: slider
        hoverEnabled: true
        onPositionChanged: if (mouseX >= handle.x && mouseX <= handle.x + handle.width) {
                               popup.visible = true;
                           }
                           else {
                               popup.visible = false;
                           }
        onExited: popup.visible = false
    }

    Rectangle {
        id: handle
        smooth: true
        y: 2
        width: 5 * slider.width / (slider.maximum - slider.minimum)
        height: slider.height - 4
        radius: 6
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "lightgray"
            }

            GradientStop {
                position: 1.0
                color: "gray"
            }
        }

        MouseArea {
            id: mouseH
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 2
            drag.maximumX: slider.xMax + 2
            acceptedButtons: Qt.LeftButton
            onPositionChanged: {
                slider.value = ((maximum - minimum) * (handle.x - 2) / xMax + minimum) * coeff;
                popup.text = Math.round(slider.value * 10000) / 10000;
            }
     }
}

    Text {
        id: popup
        visible: false
        anchors.bottom: handle.top
        anchors.left: handle.left
        text: slider.objectName + slider.value
    }
}
