uniform lowp float FadeScalar;
uniform lowp vec3 FadeColor;

void main(void) {
    gl_FragColor = vec4(FadeColor.r, FadeColor.g, FadeColor.b, FadeScalar);
}