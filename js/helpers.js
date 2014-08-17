.pragma library

function pad (number, max, decimal) {
    return (Array(max + 1).join(' ') + number).slice(-max - decimal - 2);
}

function clamp (value, minValue, maxValue) {
    return value > maxValue ? maxValue : value < minValue ? minValue : value;
}

function createObjects(objectsDataDict, objectsDict, parent, componentPath) {
    objectsDict[parent] = objectsDict[parent] || { };
    var objectsWithParent = objectsDict[parent];

    for (var object in objectsDataDict) {
        // create new component if there is no one with such id
        if (!objectsWithParent[object]) {
            objectsWithParent[object] = Qt.createComponent(componentPath).createObject(parent);
        }

        var objectWithParent = objectsWithParent[object];

        for (var prop in objectsDataDict[object]) {
            if (objectWithParent.hasOwnProperty(prop)) {
                objectWithParent[prop] = objectsDataDict[object][prop];
            }
        }
    }
}
