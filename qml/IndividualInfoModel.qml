import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict
import "../js/helpers.js" as Helpers
import "../js/settings.js" as Settings

ListModel {
    id: individualListModel;

    Component.onCompleted: {
        function populateListElements(ptps, measures) {
            var qmlElements = [];
            var points = Settings.Points[individualInfo.modelID];
            for (var ptp in ptps) {
                var pointsOfPTP = ptps[ptp];

                for (var i = 0; i !== pointsOfPTP.length; ++ i) {
                    var qmlElement = {};
                    qmlElement["from"] = measures[ptp].name;
                    qmlElement["to"] = measures[pointsOfPTP[i]].name;

                    // if selected point has from and to points, which are measured - assign them to appropriate ps
                    var positionFrom = undefined;
                    var positionTo = undefined;
                    if (points && points[ptp] && points[pointsOfPTP[i]]) {
                        positionFrom = points[ptp].position;
                        positionTo = points[pointsOfPTP[i]].position;
                    }
                    // if we know position of from and to points, calc and show it
                    qmlElement["measure"] =
                            (positionFrom && positionTo) ? (+(positionTo.minus(positionFrom).length()).toFixed(8)).toString() : "Нет данных";

                    qmlElements.push(qmlElement);
                }
            }

            return qmlElements;
        }

        var measures = PointsDict.pointsDict.measures;
        var groups = PointsDict.pointsDict.groups;
        var groupOrder = PointsDict.groupsOrder.IndividualInfo;

        for (var i = 0; i !== groupOrder.length; ++ i) {
            var group = groups[groupOrder[i]];
            append({
                       "itemId" : groupOrder[i],
                       "itemName" : group.name,
                       "itemText" : group.distances.header,
                       "subItems" : populateListElements(group.distances.point2point, measures),
                       "grouped" : groupOrder[i] !== "ungrouped",
                       "elementHeight" : 0
                   });
        }
    }
}
