#version 410
layout(location = 0) in highp vec4 vertex;

uniform highp mat4 vp;

void main(void) {
    gl_Position = vp * vertex;
}
