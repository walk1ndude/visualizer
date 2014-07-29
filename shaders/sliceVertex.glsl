#version 410
layout(location = 0) in highp vec3 vertex;
layout(location = 1) in highp vec3 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

out vec4 fragPos;

void main(void) {
    gl_Position = projection * view * vec4(vertex, 1.0f);

    vec3 texVec = vec3(0.5, 0.5, 0.5);
    fragPos = scale * model * vec4(tex - texVec, 1.0f) + vec4(texVec, 1.0f);
}
