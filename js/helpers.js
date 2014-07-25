.pragma library

function pad (number, max, decimal) {
    return (Array(max + 1).join(' ') + number).slice(-max - decimal - 2);
}

function clamp (value, minValue, maxValue) {
    return value > maxValue ? maxValue : value < minValue ? minValue : value;
}

var insertElementsIndivGroupTemplate = "import QtQuick 2.3; ListElement {property string from: {fromP}; property string to: {toP};}";
//var insertElementsIndivGroupTemplate = "import QtQuick 2.3; ListElement {}";

var insertElementsSideBarTemplate = "import QtQuick 2.3; ListElement { }";
