#version 330 core
in highp vec4 vertex;
in highp vec3 normal;
in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

uniform highp vec4 specular;
uniform highp float shininess;

uniform highp mat3 normalMatrix;

smooth out highp vec4 fragPos;
smooth out highp vec4 fragNormal;
smooth out highp vec4 vertexPos;

void main(void) {
    vertexPos = view * vertex;

    gl_Position = projection * vertexPos;

    fragNormal = vec4(normalize(normalMatrix * normal), 0);

    vec4 texVec = vec4(0.5, 0.5, 0.5, 1.0);
    fragPos = scale * model * (tex - texVec) + texVec;

}
