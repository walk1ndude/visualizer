import QtQuick 2.3;

import ParserTools 1.0;

DicomReader {
    id: dicomReader;

    property variant viewer: ({});

    onFinished: {
        viewer.processModel(model);
        toggleDocks();
    }
}
