#version 410
layout(triangles) in;
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
    
    int vertexCount = (gl_in[0].gl_Position == gl_in[1].gl_Position
                       && gl_in[1].gl_Position == gl_in[2].gl_Position) ? 1 : gl_in.length();
    
    for(int i = 0; i != vertexCount; ++ i) {
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
    
    if (vertexCount > 1) {
        for(int i = 0; i != vertexCount; ++ i) {
            frag.fColor = vertices[i].vColor;
            gl_Position = mvp * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
