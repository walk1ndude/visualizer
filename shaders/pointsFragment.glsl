#version 410
layout(location = 0) in highp vec4 oColor;

out highp outColor

void main(void) {
    outColor = oColor;
}
