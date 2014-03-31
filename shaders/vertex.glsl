#version 130
in highp vec4 vertex;
in highp vec4 normal;
in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 modelView;

out highp vec4 fragTex;
out highp vec4 aColor;

void main(void) {
    gl_Position = projection * modelView * vertex;
    //fragTex = modelView * (tex + vec4(0.5)) + vec4(-0.5);
    fragTex = tex;
}
