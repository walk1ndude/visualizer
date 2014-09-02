#version 410
layout(triangles) in;
layout(triangle_strip, max_vertices = 75) out;

#define HALF_SIDE 0.01f

in vData {
    highp vec4 vColor;
} vertices[];

uniform highp mat4 mvp;

out fData {
    highp vec4 fColor;
} frag;

void drawCube(const int i);

void drawCube(const int i) {
    // 14 vertices for cube
    const int sideXY = 0x285E31C7;
    const int sideZ = 0x00003D0B;

    frag.fColor = vertices[i].vColor;

    for (int j = 0; j != 14; ++ j) {
        gl_Position = mvp * (
                             gl_in[i].gl_Position +
                             vec4(
                                  (bool(sideXY & (1 << j)) ? 1 : -1) * HALF_SIDE,
                                  (bool(sideXY & (1 << (j + 16))) ? 1 : -1) * HALF_SIDE,
                                  (bool(sideZ & (1 << j)) ? 1 : -1) * HALF_SIDE,
                                  0.0f)
                             );
        EmitVertex();
    }

    EndPrimitive();
}

void main(void) {
    int i = 0;
    int l = gl_in.length();

    drawCube(i);

    while (i < l - 1 && gl_in[i].gl_Position != gl_in[i + 1].gl_Position) {
        drawCube(++ i);
    }

    if (i == l - 1) {
        for (int k = 0; k != gl_in.length(); ++ k) {
            frag.fColor = vertices[k].vColor;
            gl_Position = mvp * gl_in[k].gl_Position;
            EmitVertex();
        }

        EndPrimitive();
    }
}
