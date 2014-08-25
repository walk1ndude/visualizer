#version 410

bool needToRender(vec3 point, vec2 xRange, vec2 yRange, vec2 zRange, vec2 xab, vec2 yab, vec2 zab) {
    return point.s >= xRange.s * xab.s + xab.t
           && point.s <= xRange.t * xab.s + xab.t
           && point.t >= yRange.s * yab.s + yab.t
           && point.t <= yRange.t * yab.s + yab.t
           && point.p >= zRange.s * zab.s + zab.t
           && point.p <= zRange.t * zab.s + zab.t;
}
