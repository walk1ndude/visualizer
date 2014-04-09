#version 330 core
in highp vec4 vertex;
in highp vec4 normal;
in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

uniform highp mat4 normalMatrix;

uniform highp vec3 lightPos;

smooth out highp vec4 fragTex;

smooth out float fragNormal;

void main(void) {
    vec4 vertexPos = view * vertex;

    gl_Position = projection * vertexPos;

    vec4 texVec = vec4(0.5, 0.5, 0.5, 1.0);
    fragTex = scale * model * (tex - texVec) + texVec;
}
