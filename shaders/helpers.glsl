#version 410

uniform highp sampler2D points;

uniform highp int pointsCount;

bool needToRender(const vec3 position,
                  const vec2 xRange, const vec2 yRange, const vec2 zRange,
                  const vec2 xab, const vec2 yab, const vec2 zab) {
    return position.x >= xRange.s * xab.s + xab.t
           && position.x <= xRange.t * xab.s + xab.t
           && position.y >= yRange.s * yab.s + yab.t
           && position.y <= yRange.t * yab.s + yab.t
           && position.z >= zRange.s * zab.s + zab.t
           && position.z <= zRange.t * zab.s + zab.t;
}

vec4 highlightColor(const vec3 position) {
    vec4 color = vec4(0.0f);

    vec4 pointPos;
    for (int i = 0; i != pointsCount; ++ i) {
        // x, y, z - point coords, w - color radius
        pointPos = texture(points, vec2(0, i));

        if ((position.x - pointPos.x) * (position.x - pointPos.x) +
            (position.y - pointPos.y) * (position.y - pointPos.y) +
            (position.z - pointPos.z) * (position.z - pointPos.z) < pointPos.w * pointPos.w) {
            color += texture(points, vec2(1, i));
        }
    }

    return color;
}
