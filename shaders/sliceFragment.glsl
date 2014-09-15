#version 410
layout(location = 0) in highp vec4 fragPos;

uniform highp sampler3D texHead;

uniform highp mat4 normalMatrix;

uniform highp vec4 eye;

layout(location = 0) out highp vec4 fragColor;

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const vec3 positionModel);

bool needToRender(const vec3 point,
                  const vec2 xab, const vec2 yab, const vec2 zab);


void main(void) {
    if (needToRender(fragPos.xyz, vec2(0.5f, 0.5f), vec2(0.5f, 0.5f), vec2(0.5f, 0.5f))) {
        vec4 headColor = texture(texHead, fragPos.stp).rrrr;

        if (headColor.r > 0.05f) {
            fragColor = calcFragColor(fragPos, fragPos, headColor, fragPos.xyz);

            // for greater good
/*
            if (headColor.r > 0.75 && headColor.r < 0.85 && fragPos.p > 0.45 && fragPos.p < 0.8) {
            //if (headColor.r > 0.82) {//&& fragPos.p < 0.8 && fragPos.s > 0.33 && fragPos.s < 0.67) {
                fragColor.a *= (0.01 * headColor.r);
            }
            else {
                fragColor.a *= 0.05;
            }
*/
            //fragColor.a = clamp(fragColor.a, 0.0, 1.0);

        }
        else {
            discard;
        }
    }
    else {
        discard;
    }
}
