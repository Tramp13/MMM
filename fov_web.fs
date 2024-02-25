#version 100

precision mediump float;

struct FOV {
    vec2 pos;
    float radius;
};

uniform FOV fov;

void main() {
    float alpha = 1.0;

    vec2 pos = vec2(gl_FragCoord.x, gl_FragCoord.y);
    float d = distance(pos, fov.pos);

    if (d > fov.radius) {
        alpha = 1.0;
    } else {
        alpha = d / fov.radius;
    }

    gl_FragColor = vec4(0, 0.46, 0.17, alpha);
}
