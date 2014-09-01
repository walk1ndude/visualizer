.pragma library

function pad (number, max, decimal) {
    return (Array(max + 1).join(' ') + number).slice(-max - decimal - 2);
}

function clamp (value, minValue, maxValue) {
    return value > maxValue ? maxValue : value < minValue ? minValue : value;
}

var pointInGroups = {};

function addInPointToGroup(name, groups) {
    var nameInGroups = [];

    for (var group in groups) {
        if (group !== "ungrouped") {
            var ptp = groups[group].distances.point2point;
            for (var point in ptp) {
                var i = 0;
                var l = ptp[point].length;

                for (i = 0; i !== l && ptp[point][i] !== name; ++ i);

                if (i < l || point === name) {
                    nameInGroups.push(group);
                    break;
                }
            }
        }
    }

    pointInGroups[name] = nameInGroups;
}
