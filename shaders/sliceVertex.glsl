#version 410
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

layout(location = 0) out highp vec4 fragPos;

void main(void) {
    gl_Position = projection * view * vertex;

    vec4 texVec = vec4(0.5f, 0.5f, 0.5f, 0.0f);
    fragPos = scale * model * (tex - texVec) + texVec;
}
