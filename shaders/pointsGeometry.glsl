#version 410
layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

#define HALF_SIDE 0.05f

in vData {
    highp vec4 vColor;
} vertices[];

uniform highp mat4 mvp;

out fData {
    highp vec4 fColor;
} frag;

void main(void) {
    // 14 vertices for cube
    const int sideXY = 0x285E31C7;
    const int sideZ = 0x00003D0B;
    
    for(int i = 0; i != gl_in.length(); ++ i) {
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
}
