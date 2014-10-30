#version 410

layout(location = 0) in highp vec4 vColor;

layout(location = 0) out highp vec4 fColor;

void main(void) {
    fColor = vColor;
}
