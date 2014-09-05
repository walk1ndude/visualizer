#version 410
layout(triangles) in;
layout(triangle_strip, max_vertices = 15) out;

#define HALF_SIDE 16.0f

in vData {
    highp vec4 vColor;
} vertices[];

uniform highp mat4 mvp;

uniform mediump vec4 viewportSize;

out fData {
    highp vec4 fColor;
    highp vec2 fPos;
    flat int isBillboard;
} frag;

void drawMarker(const int i);

void drawMarker(const int i) {
    frag.fColor = vertices[i].vColor;
    frag.isBillboard = 1;

    vec4 vertex;

    for (int j = 0; j != 4; ++ j) {
        vertex = mvp * gl_in[i].gl_Position;

        frag.fPos = vec2(
                    (j / 2 == 1 ? 1 : -1),
                    (j % 2 == 0 ? -1 : 1)
                    );

        lowp float radius = max(HALF_SIDE / viewportSize.x * vertex.w,
                                HALF_SIDE / viewportSize.y * vertex.w);

        gl_Position = vertex + vec4(frag.fPos.x * radius, frag.fPos.y * radius,
                                    0.0f, 0.0f);

        frag.fPos *= 0.5f;
        frag.fPos += 0.5;

        EmitVertex();
    }

    EndPrimitive();
}

void main(void) {
    int i = 0;
    int l = gl_in.length();

    bool needToDraw = true;

    while (needToDraw) {
        drawMarker(i);
        needToDraw = (i < l - 1 && gl_in[i].gl_Position != gl_in[i + 1].gl_Position);

        ++ i;
    }

    if (i == l) {
        for (int k = 0; k != gl_in.length(); ++ k) {
            frag.fColor = vertices[k].vColor;
            frag.isBillboard = 0;

            gl_Position = mvp * gl_in[k].gl_Position;
            EmitVertex();
        }

        EndPrimitive();
    }
}
