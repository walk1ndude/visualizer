.pragma library

function pad (number, max, decimal) {
    return (Array(max + 1).join(' ') + number).slice(-max - decimal - 2);
}

function clamp (value, minValue, maxValue) {
    return value > maxValue ? maxValue : value < minValue ? minValue : value;
}
