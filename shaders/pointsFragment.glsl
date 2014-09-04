#version 410
in fData {
    highp vec4 fColor;
    flat int isBillboard;
} frag;

out highp vec4 fragColor;

void main(void) {
    fragColor = frag.fColor;

    if (frag.isBillboard == 1) {
        vec3 N;

        N.xy = gl_PointCoord * 2.0 - vec2(1.0);
        float mag = dot(N.xy, N.xy);

        if (mag > 1.0) {
            //discard; // kill pixels outside circle
        }

        N.z = sqrt(1.0 - mag);

        fragColor *= vec4(1.0f);//max(0.0, dot(N, N));
    }
}
