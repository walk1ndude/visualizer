#version 410
layout(location = 0) in highp vec4 fragPos;
layout(location = 1) in highp vec4 vertPos;

uniform highp sampler3D texHead;

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
            fragColor = calcFragColor(vertPos, fragPos, headColor, fragPos.xyz);
            //fragColor += vec4(1.0);
        }
        else {
            discard;
        }
    }
    else {
        discard;
    }
}
