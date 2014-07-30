#version 410
in fData {
    highp vec4 fColor;
} frag;

out highp vec4 fragColor;

void main(void) {
    fragColor = frag.fColor;
}
