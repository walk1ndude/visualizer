function pad (number, max, decimal) {
    return (Array(max + 1).join(' ') + number).slice(-max - decimal - 2);
}
