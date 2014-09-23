#version 410
uniform highp vec4 color;

layout(location = 0) out highp vec4 fColor;

void main(void) {
    fColor = color;
}
