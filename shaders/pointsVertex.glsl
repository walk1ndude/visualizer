#version 410
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec4 color;

uniform highp mat4 vp;

out highp vec4 oColor;

void main(void) {
    gl_Position = vp * vertex;
    oColor = color;
}
