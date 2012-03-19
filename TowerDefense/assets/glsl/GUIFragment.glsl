varying lowp vec4 VertexColor;
uniform lowp float VertexAlpha;

void main(void) {
    gl_FragColor = vec4(VertexColor.rgb, VertexAlpha);
}