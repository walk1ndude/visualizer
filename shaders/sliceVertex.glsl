#version 410
in highp vec4 vertex;
in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

out vec4 fragPos;

void main(void) {
    gl_Position = projection * view * vertex;

    vec4 texVec = vec4(0.5, 0.5, 0.5, 1.0);
    fragPos = scale * model * (tex - texVec) + texVec;
}
