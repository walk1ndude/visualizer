#version 330 core
in highp vec4 vertex;
in highp vec4 normal;
in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

out highp vec4 fragTex;
out highp vec4 aColor;

void main(void) {
    gl_Position = projection * view * vertex;

    vec4 texVec = vec4(0.5, 0.5, 0.5, 1.0);

    fragTex = scale * model * (tex - texVec) + texVec;
}
