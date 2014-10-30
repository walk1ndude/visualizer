#version 410
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec4 tex;

uniform highp mat4 projection;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 viewTexture;
uniform highp mat4 scale;
uniform highp mat4 modelBillboard;

layout(location = 0) out highp vec4 fragPos;
layout(location = 1) out highp vec4 vertPos;

void main(void) {
    vertPos = projection * view * modelBillboard * vertex;

    vec4 texVec = vec4(0.5f, 0.5f, 0.5f, 0.0f);
    fragPos = scale * model * (tex - texVec) + texVec;

    gl_Position = vertPos;
}
