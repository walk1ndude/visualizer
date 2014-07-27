#version 410
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec3 color;
layout(location = 2) in highp float polygon;

uniform highp mat4 mvp;

out vData {
    highp vec3 color;
    highp float polygon;
} vertex;

void main(void) {
    gl_Position = mvp * vertex;

    vertex.color = color;
    vertex.polygon = polygon;
}
