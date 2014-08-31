#version 410
layout(location = 0) in highp vec4 pos;
layout(location = 1) in highp vec3 vertexTest;
layout(location = 2) in highp vec4 N;

uniform highp vec4 colorU;

uniform highp mat4 mvp;

layout(location = 0) out highp vec4 fragColor;


bool needToRender(const vec3 point,
                  const vec2 xab, const vec2 yab, const vec2 zab);

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const mat4 mvp);


void main(void) {
    if (needToRender(vertexTest, vec2(1.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 0.0))) {
        fragColor = calcFragColor(pos, N, colorU, mvp);
    }
    else {
        discard;
    }
}
