import QtQuick 2.3

import RenderTools 1.0

SideDockHorizontal {
    id: consoleOutput;

    heading: "Console output";

    color: "blue";

    ConsoleLogger {
        id: logger;

        onOutputChanged: {
            textEdit.text = logger.output;
        }
    }

    TextEdit {
        id: textEdit;
        readOnly: true;

        font {
            family: "Helvetica";
            pointSize: 20;
            bold: true;
        }

        textFormat: Text.RichText;

        visible: !head.collapsed;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        wrapMode: Text.WordWrap;
    }

}
