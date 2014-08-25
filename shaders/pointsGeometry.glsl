#version 410
layout(points) in;
layout(line_strip, max_vertices = 3) out;

in vData {
    highp vec4 vColor;
    //highp float vPolygon;
} vertices[];

uniform highp mat4 mvp;

out fData {
    highp vec4 fColor;
} frag;

void main(void) {
    for(int i = 0; i < gl_in.length(); i++) {
        // copy attributes
        for (int v = 0; v < 3; ++ v) {
            gl_Position = mvp * (gl_in[i].gl_Position + vec4(0.005f + v % 2, 0.005f, 0.005f, 1.0f));
            frag.fColor = vertices[i].vColor;
            // done with the vertex
            EmitVertex();
        }
    }
    EndPrimitive();
}
