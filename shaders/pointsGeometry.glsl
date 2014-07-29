#version 410
layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

in vData {
    highp vec3 color;
    highp float polygon;
} vertices[];

out fData {
    highp vec3 color;
} frag;

void main(void) {
    for (int i = 0; i < gl_in.length(); ++ i) {
        vec4 vertex = gl_in[i].gl_Position;
        for (int v = 0; v < 3; ++ v) {
            gl_Position = gl_in[i].gl_Position + vec4(vertex.x + 0.5f * (v % 2), vertex.y + 0.5f * (v % 2), vertex.z, vertex.w);
            
            frag.color = vertices[i].color;
            EmitVertex();
        }
        EndPrimitive();
    }
}
