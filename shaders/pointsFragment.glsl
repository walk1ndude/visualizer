#version 410
in fData {
    highp vec4 fColor;
    highp vec2 fPos;
    flat int isBillboard;
} frag;

uniform mediump vec4 viewportSize;

const highp vec2 center = vec2(0.5f, 0.5f);

out highp vec4 fragColor;

void main(void) {
    fragColor = frag.fColor;

    if (frag.isBillboard == 1) {
        highp float dist = distance(center, frag.fPos);

        lowp float delta = 0.001f;
        highp float alpha = smoothstep(0.45f - delta, 0.45f, dist);

        fragColor = mix(fragColor, vec4(0.0f), alpha);

        if (fragColor.r < 0.1f && fragColor.g < 0.1f && fragColor.b < 0.1f) {
            discard;
        }
    }
}
