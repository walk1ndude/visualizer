#version 410 core

in highp vec4 vertex;
in highp vec3 normal;

uniform highp mat4 mvp;

layout(location = 0) out highp vec4 pos;
layout(location = 1) out highp vec3 norm;

void main(void) {
    pos = mvp * vertex;
    norm = normal;

    gl_Position = pos;
}
