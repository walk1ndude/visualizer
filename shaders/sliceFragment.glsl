#version 410
in highp vec4 fragPos;

uniform highp sampler3D texHead;

uniform highp mat4 normalMatrix;

uniform highp vec3 stepSlices;

out highp vec4 fragColor;


vec4 sobel3(vec3 position);

vec4 sobel3(vec3 position) {
    vec3 p = vec3(0.0f);
    vec3 n = vec3(0.0f);

    float fragElem[27];

    fragElem[0] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, -stepSlices.z)).r;
    fragElem[1] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, 0.0f, -stepSlices.z)).r;
    fragElem[2] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, -stepSlices.z)).r;
    fragElem[3] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, 0.0f)).r;
    fragElem[4] = -6.0f * texture(texHead, position + vec3(-stepSlices.x, 0.0f, 0.0f)).r;
    fragElem[5] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, 0.0f)).r;
    fragElem[6] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, stepSlices.z)).r;
    fragElem[7] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, 0.0f, stepSlices.z)).r;
    fragElem[8] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, stepSlices.z)).r;

    for (int i = 9; i != 18; ++ i) {
        fragElem[0] = 0.0f;
    }

    fragElem[18] = 1.0f * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, -stepSlices.z)).r;
    fragElem[19] = 3.0f * texture(texHead, position + vec3(stepSlices.x, 0.0f, -stepSlices.z)).r;
    fragElem[20] = 1.0f * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, -stepSlices.z)).r;
    fragElem[21] = 3.0f * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, 0.0f)).r;
    fragElem[22] = 6.0f * texture(texHead, position + vec3(stepSlices.x, 0.0f, 0.0f)).r;
    fragElem[23] = 3.0f * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, 0.0f)).r;
    fragElem[24] = 1.0f * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, stepSlices.z)).r;
    fragElem[25] = 3.0f * texture(texHead, position + vec3(stepSlices.x, 0.0f, stepSlices.z)).r;
    fragElem[26] = 1.0f * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, stepSlices.z)).r;

    for (int i = 0; i != 27; ++ i) {
        if (i / 9 == 0) {
            p.x += fragElem[i];
        }

        if (i / 9 == 2) {
            n.x += fragElem[i];
        }

        if (i % 3 == 0) {
            p.y += fragElem[i];
        }

        if (i % 3 == 2) {
            n.y += fragElem[i];
        }

        if (i % 9 / 3 == 0) {
            p.z += fragElem[i];
        }

        if (i % 9 / 3 == 2) {
            n.z += fragElem[i];
        }
    }

    return vec4(normalize(n - p), 0.0f);
}

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const vec3 positionModel);

bool needToRender(const vec3 point,
                  const vec2 xab, const vec2 yab, const vec2 zab);


void main(void) {
    if (needToRender(fragPos.xyz, vec2(0.5f, 0.5f), vec2(0.5f, 0.5f), vec2(0.5f, 0.5f))) {
        vec4 headColor = texture(texHead, fragPos.stp).rrrr;

        if (headColor.r > 0.05f) {
            fragColor = calcFragColor(fragPos, sobel3(fragPos.stp), headColor, fragPos.xyz);

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
