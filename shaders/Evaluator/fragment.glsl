#version 410
in vec4 position;
uniform highp vec4 color;

layout(location = 0) out highp vec4 fColor;

void main(void) {
    //if (position.x >= 0) {
        fColor = color;
    /*}
    else {
        fColor = vec4(1.0);
    }*/
}
