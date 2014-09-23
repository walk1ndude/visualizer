import QtQuick 2.3;

import RenderTools 1.0;

Rectangle {
    id: consoleWrapper;

    height: consoleOutput.head.height;

    Sidedock {
        id: consoleOutput;

        heading: "Console output";

        color: "#880000FF";

        ConsoleLogger {
            id: logger;

            onOutputChanged: {
                textEdit.text = logger.output;
            }

            lineCount: 20;
        }

        Flickable {
            id: flick;

            width: parent.width;
            height: consoleOutput.dY - consoleOutput.head.height + 20;

            contentWidth: textEdit.paintedWidth;
            contentHeight: textEdit.paintedHeight;

            anchors {
                left: parent.left;
                right: parent.right;
                top: parent.top;

                margins: 10;
            }

            visible: !consoleOutput.head.collapsed;

            clip: true;

            function ensureVisible(r) {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX + width <= r.x + r.width)
                    contentX = r.x + r.width - width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY + height <= r.y + r.height)
                    contentY = r.y + r.height - height;
            }

            TextEdit {
                id: textEdit;

                width: flick.width;
                height: flick.height;
                focus: true;

                readOnly: true;

                font {
                    family: "Helvetica";
                    pointSize: 14;
                    bold: true;
                }

                wrapMode: TextEdit.Wrap;
                textFormat: Text.RichText;

                onCursorRectangleChanged: flick.ensureVisible(cursorRectangle);
            }
        }
    }
}
