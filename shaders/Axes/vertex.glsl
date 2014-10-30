#version 410

layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec4 color;

uniform highp mat4x4 mvp;

layout(location = 0) out highp vec4 vColor;

void main(void) {
    gl_Position = mvp * vertex;

    vColor = color;
}
