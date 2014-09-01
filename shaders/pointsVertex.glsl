#version 410
layout(location = 0) in highp vec3 vertex;
layout(location = 1) in highp vec3 color;

out vData {
    highp vec4 vColor;
} oVertex;

void main(void) {
    gl_Position = vec4(vertex, 1.0f);
    oVertex.vColor = vec4(color, 1.0f);
}
