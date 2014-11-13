#version 410
layout(location = 0) in highp vec4 fragPos;
layout(location = 1) in highp vec4 vertPos;

uniform highp sampler3D volume;

uniform highp vec4 eye;

layout(location = 0) out highp vec4 fragColor;

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const vec3 positionModel);

bool needToRender(const vec3 point,
                  const vec2 xab, const vec2 yab, const vec2 zab);


void main(void) {
    if (needToRender(fragPos.xyz, vec2(0.5f, 0.5f), vec2(0.5f, 0.5f), vec2(0.5f, 0.5f))) {
        vec4 volumeColor = texture(volume, fragPos.stp).rrrr;

        if (volumeColor.r > 0.05f) {
            if (volumeColor.r < 0.97f) {
                fragColor = calcFragColor(vertPos, fragPos, volumeColor, fragPos.xyz);
            }
            else {
                if (volumeColor.r == 1.0f) {
                    fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
                }
                else {
                    if (volumeColor.r < 0.98f) {
                        fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
                    }
                    else {
                        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
                    }
                }
            }
            fragColor += vec4(0.3);
        }
        else {
            discard;
            //fragColor = vec4(1.0f, 0.0f, 0.0f, 0.01f);
        }
    }
    else {
        discard;
        //fragColor = vec4(1.0f, 0.0f, 0.0f, 0.01f);
    }
}
