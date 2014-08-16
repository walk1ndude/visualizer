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

function show(objects) {
    for (var object in objects) {
        object.visible = true;
    }
}

function hide(objects) {
    for (var object in objects) {
        object.visible = false;
    }
}

function mouseInViewPort(mouseX, mouseY, viewports, viewport) {
    for (var viewportId in viewports) {
        var viewport = viewports[viewportId];

        if (viewport.x <= mouseX && viewport.y <= mouseY
            && viewport.x + viewport.width > mouseX
            && viewport.y + viewport.height > mouseY) {
            viewport.color = "red";
        }
        else {
            viewport.color = "green";
        }
    }
}
