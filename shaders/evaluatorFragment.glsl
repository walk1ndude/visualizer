#version 410
layout(location = 0) in highp vec4 fColor;

layout(location = 0) out highp vec4 oColor;

void main(void) {
    oColor = fColor;
}
