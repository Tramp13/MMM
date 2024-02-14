#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

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
        alpha = 0.0;
    }

    finalColor = vec4(0, 0, 0, alpha);
}
