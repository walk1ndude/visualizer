layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

in vData {
    highp vec4 color;
    highp float polygon;
} vertices[];

void main(void) {
    for (int i = 0; i < gl_in.length(); ++ i) {

    }
}
