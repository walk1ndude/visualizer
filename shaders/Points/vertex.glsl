#version 410
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec4 color;

out vData {
    highp vec4 vColor;
} oVertex;

void main(void) {
    gl_Position = vertex;
    oVertex.vColor = color;
}
