#version 330 

#define MAX_ITER 100

uniform vec2 resolution;

float sqrMag(vec2 v) {
    return v.x*v.x + v.y*v.y;
}
void main(void) {
    vec2 uv = vec2(gl_FragCoord.x / resolution.x, gl_FragCoord.y / resolution.y);
    uv -= 0.5;
    uv *= 2.0;
    uv.x -= 0.5;
    vec2 curr = uv;
    int i = 0;
    while (i++ < MAX_ITER && sqrMag(curr) < 4.0) {
        float a = curr.x;
        float b = curr.y;
        float c = curr.x;
        float d = curr.y;
        curr = vec2(a*c-b*d, a*d+b*c);
        curr += uv;
    }
    gl_FragColor = vec4(i / float(MAX_ITER), 0.0, 0.0, 1.0);
}
